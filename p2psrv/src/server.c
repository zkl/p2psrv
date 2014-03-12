#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <linux/tcp.h>
#include <linux/socket.h>
#include "server.h"
#include "pclient.h"
#include "pworker.h"
#include "log.h"


/* 新客户端上线 */
void * pserver_accept(pserver_t * server)
{
	struct sockaddr_in addr;
	struct epoll_event ev;
	int len = sizeof(struct sockaddr_in); 

	int new_fd = accept(server->listen_fd, 
		(struct sockaddr *)&addr, &len);

	if(new_fd < 0)
	{
		perror("accept");
		return 0;
	}
	else
	{
		pserver_set_no_blocking(new_fd);

		pserver_data_t * data = (pserver_data_t *)malloc(
			sizeof(pserver_data_t));

		data->listen_fd = new_fd;
		data->ptr = pclient_create(new_fd);
	
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET | EPOLLOUT | EPOLLERR;
		ev.data.ptr = data;

		// 创建客户端消息处理模块
		int ret;
		ret = epoll_ctl(server->kdpfd, EPOLL_CTL_ADD, new_fd,
			&ev);

		if(ret < 0)
			return 0;

		pserver_set_keep_alive(new_fd, 60, 5, 8);
		server->client_num++;
	}

	return 0;
}

/* 客户端事件 */
void * pserver_events(void * parm)
{
	// 客户端消息处理 调用个消息分发函数
	pserver_task_t * task = (pserver_task_t *)parm;

	int ret = 0;
	switch(task->event)
	{
	case PSERVER_NEW_CONNECT:
		pserver_accept(task->server);
		break;
	case PSERVER_SOCKET_EVENT:
		{
			pserver_data_t * pdata = task->ev.data.ptr;
			pclient_t * client =  pdata->ptr;	
			if(ret == 0 && (task->ev.events & EPOLLIN))
			{
				ret = client->proc_msg(task->server, PCLIENT_IN_EVENT, 
					client);
			}

			if(ret == 0 && (task->ev.events & EPOLLOUT))
			{
				ret = client->proc_msg(task->server, PCLIENT_OUT_EVENT, 
					client);
			}

			if(ret == 0 && (task->ev.events & EPOLLERR))
			{
				ret = client->proc_msg(task->server, PCLIENT_ERROR_EVENT, 
					client);
			}

			if(ret != 0)
			{
				log_debug("客户端下线");
				epoll_ctl(task->server->kdpfd, EPOLL_CTL_DEL, 
					pdata->listen_fd, 0);
		
				close(pdata->listen_fd);
				pclient_destroy(client);
				free(pdata);
				task->server->client_num--;
			}
			break;
		}
	}
	free(task);
}

int pserver_set_no_blocking(int fd)
{
	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) == -1))
	{
		return -1;
	}
}

int pserver_set_keep_alive(int fd, int time, int check_time, int check_cnt)
{
	int optval;
	socklen_t optlen = sizeof(optval);
	optval = 1;

	if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen))
	{
		return -1;
	}

	optval = time;

	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen))
	{
		return -2;
	}

	optval = check_time;
	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen))
	{
		return -3;
	}

	optval = check_cnt;
	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen))
	{
		return -4;
	}

	return 0;
}
	

void pserver_init(pserver_t * server, int max_listen, int port)
{
	LOG_DEBUG("initialize server.");

	server->listen_port =  port;
	server->client_num = 0;
	server->max_listen_num = max_listen;
	server->listen_fd = socket(PF_INET, SOCK_STREAM, 0);

	pworker_create(&server->worker, 10);
	ptimer_create(&server->timer);

	if(server->listen_fd == -1)
	{
		server->is_actived = 0;
		return ;
	}
	else
	{
		server->is_actived = 1;
	}

	pserver_set_no_blocking(server->listen_fd);
	bzero(&server->addr, sizeof(server->addr));

	server->addr.sin_family = PF_INET;
	server->addr.sin_port = htons(server->listen_port);
	server->addr.sin_addr.s_addr = INADDR_ANY;

	int ret ; 
	ret = bind(server->listen_fd, (struct sockaddr *)&server->addr, 
		sizeof(struct sockaddr));

	if(ret == -1)
	{
		server->is_actived = 0;
		return ;
	}

	ret = listen(server->listen_fd, server->max_listen_num);
	if(ret == -1)
	{
		server->is_actived = 0;
		return ;
	}
	
	server->kdpfd = epoll_create(MAX_EPOLL_SIZE);

	pserver_data_t * data = (pserver_data_t *)malloc(
		sizeof(pserver_data_t));

	data->listen_fd = server->listen_fd;
	data->ptr = 0;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = data;
	
	ret = epoll_ctl(server->kdpfd, EPOLL_CTL_ADD, server->listen_fd, &ev);
	if(ret < 0)
	{
			server->is_actived = 0;
			return ;
	}
}

void pserver_exec(pserver_t * server)
{
	DEBUG_SERVER_PRINT("执行服务器\n");

	int len = sizeof(struct sockaddr_in); 
	struct sockaddr_in addr;
	struct epoll_event ev;
	struct epoll_event events[MAX_EPOLL_SIZE];

	server->client_num = 1;
	while(1)
	{
		int nfds = epoll_wait(server->kdpfd, events, 
			server->client_num, -1);

		if(nfds == -1)
		{
			perror("epoll_wait");
			break;
		}

		int i;
		for(i = 0; i<nfds; i++)
		{
			pserver_task_t * task = (pserver_task_t *)malloc(
				sizeof(pserver_task_t));

			pserver_data_t * data = (pserver_data_t * )events[i].data.ptr;
			if(data->listen_fd == server->listen_fd) // 监听网络描述符
			{
				task->event = PSERVER_NEW_CONNECT;
				task->server = server;
			}
			else
			{
				task->event = PSERVER_SOCKET_EVENT;
				task->server = server;
				task->ev = events[i];
			}

			pworker_append(&server->worker, pserver_events, task);
		} // for

	} // while 
	DEBUG_SERVER_PRINT("服务器退出\n");
}

void pserver_quit(pserver_t * server)
{
	DEBUG_SERVER_PRINT("退出服务器\n");
}

void pserver_destroy(pserver_t * server)
{
	pworker_destroy(&server->worker);
	ptimer_destroy(&server->timer);

	DEBUG_SERVER_PRINT("释放服务器资源\n");
}
