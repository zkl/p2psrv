/*******************************************************************************
**                                                  
** 版    本:  v 1.3.0                                                  
** 功    能:  定义链表的数据结构
** 创    期:  Sat. 3 Mar 2012  修改于 Wed. 26 Feb 2014 
** 版    权:  
** 作    者:  张昆伦 
** 备    注:                                              
*******************************************************************************/

#ifndef LIST_H
#define LIST_H

typedef struct _linked_list_node_t_
{
	void * d;
	struct _linked_list_node_t_ * prev;
	struct _linked_list_node_t_ * next;
}linked_list_node_t;

typedef struct _linked_list_
{
	int size;
	struct _linked_list_node_t_ * head;
	struct _linked_list_node_t_ * last;
}linked_list_t;


/*******************************************************************************
**                      Wed. 26 Feb 2014  更新                                */

linked_list_t *      linked_list_create();
void                 linked_list_free(linked_list_t * list);
int                  linked_list_size(linked_list_t * list);
void *               linked_list_data(linked_list_node_t * node);
linked_list_node_t * linked_list_first(linked_list_t * list);
linked_list_node_t * linked_list_last(linked_list_t * list);
linked_list_node_t * linked_list_next(linked_list_node_t * node);
linked_list_node_t * linked_list_previous(linked_list_node_t * node);
linked_list_node_t * linked_list_remove(linked_list_t * list, 
						linked_list_node_t * node);
linked_list_node_t * linked_list_insert(linked_list_t * list, 
						linked_list_node_t * node, void * data);


#endif
