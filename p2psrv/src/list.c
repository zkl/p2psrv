#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include "list.h"

/************************************************************************/
/* 功  能：比较两个data_t结构体是否相等                                   */
/* 返回值：相等返回0，否者返回非0                                       */
/* 日  期：2012年10月11日                                               */
/* 版  本：v1.0                                                         */
/* 备  注：                                                             */
/************************************************************************/
int list_datacmp(data_t des, data_t src)
{
	return 1;
}

/************************************************************************/
/* 功  能：创建空的链表表                                               */
/* 返回值：返回空的链表的结构体                                         */
/* 日  期：2012年03月14日                                               */
/* 版  本：v1.1                                                         */
/* 备  注：                                                             */
/************************************************************************/
list_t * list_create(void)
{
	list_t * list = (list_t *)malloc(sizeof(list_t));
	memset(list, 0, sizeof(list_t));
	return list;
}

/************************************************************************/
/* 功  能：销毁链表，释放链表中所有的节点，并对每个data指针回调         */
/*         callback函数                                                 */
/* 返回值：                                                             */
/* 日  期：2012年03月14日                                               */
/* 版  本：v1.1                                                         */
/* 备  注：                                                             */
/************************************************************************/
void list_destroy(list_t * list)
{
	list_node_t * node = list->head;
	int id = 0;
	while(list->head)
	{
		node = node->next;
		free(list->head);
		list->head = node;
		id++;
	}
	free(list);
}

/************************************************************************/
/* 功  能：在链表list中新增一个节点，data指向节点数据项                 */
/* 返回值：                                                             */
/* 日  期：2012年03月14日                                               */
/* 版  本：v1.0                                                         */
/* 备  注：                                                             */
/************************************************************************/
void list_append(list_t * list, data_t data)
{
	list_node_t *node = (list_node_t *)malloc(sizeof(list_node_t));
	memset(node, 0, sizeof(list_node_t));
	node->data = data;
	list->size++;
	
	if(!list->head)
		list->head = node;

	node->prev = list->last;

	if(list->last)
		list->last->next = node;

	list->last = node;
}

void list_insertl2 (list_t * list, data_t data, int position)
{
	list_node_t * pnode = list_nodeat(list, position-1);
	if(position >= list->size)
		list_append(list, data);
	else
		list_insert(list, pnode, data);
}

void list_insert(list_t * list, list_node_t * pnode, data_t data)
{
	list_node_t * node = (list_node_t *)malloc(sizeof(list_node_t));
	node->data = data;

	if(pnode == 0)
	{
		node->next = list->head;

		if(list->head)
			list->head->prev = node;
		else
			list->last = node;

		node->prev = 0;
		list->head = node;
	}
	else
	{
		node->next = pnode->next;
		pnode->next = node;
		node->prev = pnode;

		if(node->next)
			node->next->prev = node;
		else
			list->last = node;
	}
	list->size++;
}

/************************************************************************/
/* 功  能：在list中删除node指向的节点                                   */
/* 返回值：                                                             */
/* 日  期：2012年03月14日                                               */
/* 版  本：v1.2                                                         */
/* 备  注：该函数并不会释放node和data指针指向的内存，调用前需要手动释放 */
/*         data指向的内存空间                                           */
/************************************************************************/
list_node_t * list_delnde(list_t * list, list_node_t * node)
{
	if(!list || !node)
		return 0;

	/* 去掉错误验证，增加风险*/
	/* 在list_t中查找node节点如果找到则继续执行，找不到直接返回 */
	//for(; tnode->next; tnode = tnode->next)
	//{
	//	if(node == tnode)
	//		break;
	//}

	//if(tnode != node)
	//	return ;


	if(node == list->last)
		list->last = node->prev;

	if(node == list->head)
		list->head = node->next;

	if(node->prev)
		node->prev->next = node->next;
	
	if(node->next)
		node->next->prev = node->prev;

	list->size--;

	list_node_t * next_node = node->next;

	node->prev = 0;
	node->next = 0;

	free(node);

	return next_node;
}


/************************************************************************/
/* 功  能：返回list链表中第i个节点的指针                                */
/* 返回值：如果存在第i个节点，则返回其指针，否则返回NULL                */
/* 日  期：2012年03月14日                                               */
/* 版  本：v1.1                                                         */
/* 备  注：下标从0开始                                                  */
/************************************************************************/
list_node_t * list_nodeat (list_t * list, int position)
{
	list_node_t *node = 0;
	
	if(list->size <= 0 || position >= list->size || position < 0)
		return 0;

	if(position > list->size/2)  // 如果i位于链表的后半部分
	{
		int j = 0;
		node = list->last;

		for(j=list->size-1; j>position; j--)
		{
			if(node)
				node = node->prev;
		}
	}
	else                 // i位于链表的前半部分
	{
		int j=0;
		node = list->head;
		for(j=0; j<position; j++)
		{
			if(node)
				node = node->next;
		}
	}
	return node;
}


int list_nodepos(list_t * list, list_node_t * node)
{
	int i=0;
	list_node_t *tnode = list->head;

	if(list->size <= 0)
		return -1;

	for(i=0; i<list->size; i++)
	{
		if(tnode == node)
			return i;
		tnode = tnode->next;
	}
	return -1;
}

void list_info(list_t * list)
{
/*
	printf("\n    列表信息\n");
	printf("list->size = %d\n", list->size);
	printf("list->head = %X\n", list->head);
	printf("list->last = %X\n", list->last);

	list_node_t * node;
	for(node = list->head; node; node = node->next)
	{
		printf("list element : %X\n", node->data);
	}
*/
}

