/*
 * cache.h - prototpyes and definitions for cache 
 * for Part 3 of proxylab from CMU with CS:APP3e
 */
/* $begin cache.h */
#ifndef __CACHE_H__
#define __CACHE_H__

#include <stdio.h>
#include "csapp.h"

/* macros and constants */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400



/* 
 * node_c - each node of cache_list(doubly linked list)
 * c_key    : requested line (uri)
 * c_hdr    : resp. header 
 * c_val    : pointer to memory of contents (malloced)
 * obj_size : size of each obj
 * recent   : one other cache accessed more recently (toward HEAD)
 * past     : one other cache accessed at more past time (toward TAIL)
 */
/* $begin node_c */
typedef struct node_c{
    char c_key[MAXLINE];  
    char c_hdr[MAXLINE];  // hdr 추가도 해야함 (sprintf로 다 담아서 한번에 전달하는 방법 고려)  
    char *c_val; 
    int obj_size;           
    struct node_c *recent; 
    struct node_c *past; 
} node_c;
/* $end node_c */

/* 
 * cache list - doubly linked list made up with node_c
 * cache_size : total cache size
 * head       : top of the list -> would be popped
 * tail       : bottom of the list -> would be popped left
 */
/* $begin cache_list */
typedef struct {
    int cache_size;
    node_c *head;
    node_c *tail;
} cache_list;
/* $end cache_list */

/*
 * myarg - struct to be transferred to thread
 * arg_fd : connfdp
 * arg_cache : c_list 
 */
typedef struct {
    int arg_fdp;
    cache_list *arg_cache;
} myarg;

/* functions for Cache list */

void *init(cache_list *c_list);
void popleft(cache_list *c_list);
void push(cache_list *c_list, char *uri, char *srcp, char *hdr, int obj_size);
void *drag_to_top(cache_list *c_list, node_c *cache);
node_c *search(cache_list *c_list, char *uri);
int cache_be_exceeded(cache_list *c_list, int obj_size);

#endif /* __CACHE_H__ */
/* $end cache.h */