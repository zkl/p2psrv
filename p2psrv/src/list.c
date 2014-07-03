#include <malloc.h>
#include <string.h>
#include "list.h"


/*******************************************************************************
** 版  本： v 1.1     
** 功  能： 创建链表
** 入  参： 
** 返回值：                                   
** 备  注： 需要调用 linked_list_delete 删除对象
*******************************************************************************/
linked_list_t * linked_list_create(void)
{
	linked_list_t * list = (linked_list_t *)malloc(sizeof(linked_list_t));
	memset(list, 0, sizeof(linked_list_t));
	return list;
}


/* 删除列表 */
void linked_list_free(linked_list_t * list)
{
	linked_list_node_t * node = list->head;
	while(list->head)
	{
		node = node->next;
		free(list->head);
		list->head = node;
	}
	free(list);
}

/* 获取数据 */
void * linked_list_data(linked_list_node_t * node)
{
	return node->d;
}

int linked_list_size(linked_list_t * list)
{
	return list->size;
}

/* 第一个位置 */
linked_list_node_t * linked_list_first(linked_list_t * list)
{
	return list->head;
}

/* 最后一个位置 */
linked_list_node_t * linked_list_last (linked_list_t * list)
{
	return list->last;
}

/* 下一个一个位置 */
linked_list_node_t * linked_list_next (linked_list_node_t * node)
{
	return node? node->next: 0;
}

/* 上一个一个位置 */
linked_list_node_t * linked_list_previous(linked_list_node_t * node)
{
	return node? node->prev: 0;
}

/*******************************************************************************
** 版  本： v 1.1     
** 功  能： 删除一个节点
** 入  参：
** 返回值： 返回下一个节点                      
** 备  注：   
*******************************************************************************/
linked_list_node_t * linked_list_remove(linked_list_t * list, 
	linked_list_node_t * node)
{
	linked_list_node_t * next_node = node->next;

	if(node == list->last)
		list->last = node->prev;

	if(node == list->head)
		list->head = node->next;

	if(node->prev)
		node->prev->next = node->next;

	if(node->next)
		node->next->prev = node->prev;

	list->size--;

	free(node);

	return next_node;
}

/*******************************************************************************
** 版  本： v 1.1     
** 功  能： 插入数据
** 入  参：
** 返回值： 返回新插入的节点                                
** 备  注：   
*******************************************************************************/
linked_list_node_t * linked_list_insert(linked_list_t * list, 
	linked_list_node_t * node, void * data)
{
	linked_list_node_t * new_node = (linked_list_node_t *)malloc(
		sizeof(linked_list_node_t));

	new_node->d = data;
	new_node->prev = 0;
	new_node->next = 0;

	if(node == 0)
	{
		new_node->prev = 0;
		new_node->next = list->head;

		if(list->head)
		{	
			list->head->prev = new_node;
		}

		list->head = new_node;

		if(list->last == 0)
			list->last = new_node;
	}
	else
	{
		new_node->next = node->next;
		new_node->prev = node;

		node->next = new_node;

		if(new_node->next) // 存在下一个节点
			new_node->next->prev = new_node;
		else
			list->last = new_node;
	}

	list->size++;

	return new_node;
}
