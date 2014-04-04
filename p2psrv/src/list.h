/************************************************************************/
/* 功  能：定义链表的数据结构                                           */
/* 日  期：2012年03月14日                                               */
/* 修  改：2014年04月04日                                               */
/* 版  本：v1.2.4 C 版                                                  */
/* 备  注：                                                             */
/************************************************************************/

#ifndef list_t_H
#define list_t_H

typedef void * data_t;

typedef struct _list_node_
{
	data_t data;
	struct _list_node_ *prev;
	struct _list_node_ *next;
}list_node_t;

typedef struct _list_
{
	int size;
	struct _list_node_ *head;
	struct _list_node_ *last;
}list_t;


/* 创建链表 */
list_t * list_create(void);

/* 根据节点获取位置 */
int list_nodepos(list_t * list, list_node_t * node);

/* 根据位子获取节点 */
list_node_t * list_nodeat(list_t * list, int pos);

/* 插入节点 */
void list_insert(list_t * list, list_node_t * pnode, data_t data);

void list_append(list_t * list, data_t data);

/* 销毁链表 */
void list_destroy(list_t * list);

/* 删除节点 */
list_node_t * list_delnde (list_t * list, list_node_t * node);

void list_info(list_t * list);

#endif
