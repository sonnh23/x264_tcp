#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
struct vid_obj{
	char* name;
	char* path_mp4;
	char* path_264;
	long size_264;
	long size_mp4;
	bool encoded;
};
typedef struct vid_obj vid_obj_t;

/** Node in the linked list. */
struct ll_node {
  struct ll_node *next;
  struct ll_node *prev;
  vid_obj_t *object;
};
typedef struct ll_node ll_node_t;

/** A linked list. */
struct linked_list {
  ll_node_t *head;
  ll_node_t *tail;
  unsigned int length;
};
typedef struct linked_list linked_list_t;

linked_list_t *ll_create();

void ll_destroy(linked_list_t *list);

ll_node_t *ll_add(linked_list_t *list, void *object);

ll_node_t *ll_add_front(linked_list_t *list, void *object);

ll_node_t *ll_add_after(linked_list_t *list, ll_node_t *node, void *object);

void *ll_remove(linked_list_t *list, ll_node_t *node);

ll_node_t *ll_find(linked_list_t *list, void *object);

ll_node_t *ll_front(linked_list_t *list);

ll_node_t *ll_back(linked_list_t *list);

unsigned int ll_length(linked_list_t *list);

vid_obj_t* ll_find_vid_name(linked_list_t *list, char* vid_name);

void ll_print_vid_name(linked_list_t* list);
#endif