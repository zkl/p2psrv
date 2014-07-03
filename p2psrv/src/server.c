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

void * pserver_accept (void * server);

void * pserver_online (void * server);
void * pserver_offline(void * client);
void * pserver_datain (void * client);
void * pserver_dataot (void * client);
void * pserver_timeout(void * client);
void * pserver_error  (void * client);

int    pserver_set_keep_alive(int fd, int time, int check_time, 
			int check_cnt);
int    pserver_set_no_blocking(int fd);

/* 新客户端上线 */
void * pserver_accept(void * server)
{
	pserver_t * srv = (pserver_t *)server;

	struct sockaddr_in addr;
	struct epoll_event ev;
	int len = sizeof(struct sockaddr_in); 

	int new_fd = accept(srv->listen_fd, (struct sockaddr *)&addr, 
		(socklen_t *)&len);

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

		data->fd = new_fd;
		data->client = pclient_create(new_fd, srv, data);
	
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET | EPOLLOUT | EPOLLERR;
		ev.data.ptr = data;

		// 创建客户端消息处理模块
		int ret;
		ret = epoll_ctl(srv->kdpfd, EPOLL_CTL_ADD, new_fd,
			&ev);

		if(ret < 0)
			return 0;

		pserver_set_keep_alive(new_fd, 60, 5, 8);
		srv->client_num++;
	}

	return 0;
}

void * pserver_datain(void * client)
{
	pclient_data_in((pclient_t *)client);
	return 0;
}

void * pserver_dataot(void * client)
{
	pclient_data_out((pclient_t *)client);	
	return 0;
}

/*
void pserver_outline(pserver_t * server, void * client)
{
	//pserver_data_t 忘记删除
	pclient_t * c = (pclient_t *)client;
	epoll_ctl(server->kdpfd, EPOLL_CTL_DEL, c->listen_fd, 0);
	server->client_num--;
	free(pclient_srv_data(client));
}
*/

int pserver_set_no_blocking(int fd)
{
	if(fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) == -1))
		return -1;

	return 0;
}

int pserver_set_keep_alive(int fd, int time, int check_time, 
	int check_cnt)
{
	int optval;
	socklen_t optlen = sizeof(optval);
	optval = 1;


	if(setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &optval, optlen))
		return -1;

	optval = time;

	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPIDLE, &optval, optlen))
		return -2;

	optval = check_time;
	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPINTVL, &optval, optlen))
		return -3;

	optval = check_cnt;
	if(setsockopt(fd, IPPROTO_TCP, TCP_KEEPCNT, &optval, optlen))
		return -4;

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

	data->fd = server->listen_fd;
	data->client = 0;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = data;
	
	ret = epoll_ctl(server->kdpfd, EPOLL_CTL_ADD, server->listen_fd,
		&ev);
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
		/* 删除下线的用户信息 */
		/* 删除任务，和时钟 */
		int nfds = epoll_wait(server->kdpfd, events, 
			server->client_num, -1);

		if(nfds == -1)
			break;

		int i;
		for(i = 0; i<nfds; i++)
		{
			pserver_data_t * data = 
				(pserver_data_t * )events[i].data.ptr;

			// 监听网络描述符
			if(data->fd == server->listen_fd)
			{
				pworker_append(&server->worker, pserver_accept, 
					server);
			}
			else
			{
				if(events[i].events & EPOLLIN)
					pworker_append(&server->worker, pserver_datain,
						data->client);		

				if(events[i].events & EPOLLOUT)
					pworker_append(&server->worker, pserver_dataot, 
						data->client);		
	
				if(events[i].events & EPOLLERR)
						;
			}
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

