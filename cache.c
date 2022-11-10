/*
 * cache.c - Functions for Part 3 of proxylab from CMU with CS:APP3e
 */
/* $begin cache.c */
#include "cache.h"
/****************************
 * functions for Cache list 
 ****************************/

/* init - initailize a cache_list  */
void *init(cache_list *c_list){
    c_list -> cache_size = 0;
    c_list -> head = NULL;
    c_list -> tail = NULL;
}


/* 
 * cache_be_exceeded - check whether cache_list will be exceeded 
 * when a new cache come in
 */
int cache_be_exceeded(cache_list *c_list, int obj_size){
    return ((c_list -> cache_size) + obj_size > MAX_CACHE_SIZE);
}


/* push - new cache come in */
void push(cache_list *c_list, char *uri, char *srcp, char* hdr, int obj_size){
    while(cache_be_exceeded(c_list, obj_size)){
        popleft(c_list);
    }
   
    node_c *tmp = (node_c *)Malloc(sizeof(node_c));
    strcpy(tmp->c_key, uri);
    strcpy(tmp->c_hdr, hdr);
    tmp->c_val = srcp;
    tmp->obj_size = obj_size;
    tmp->past = c_list->head;
    tmp->recent = NULL;

    if(c_list->tail == NULL) c_list->tail = tmp;
    c_list->head = tmp;
    c_list->cache_size += obj_size;
}

/* popleft - pop the tail */
void popleft(cache_list *c_list){
    node_c *pop_target = c_list->tail;
    c_list->cache_size -= pop_target->obj_size;
    c_list->tail = pop_target->recent;
    c_list->tail->past = NULL;
    Free(pop_target->c_val);
    Free(pop_target);
}

/* drag_to_top - make the cache top (head) of the linked list */
void *drag_to_top(cache_list *c_list, node_c *cache){
    if(c_list->head == cache) return;
    cache->recent->past = cache->past;
    if(c_list->tail == cache) return;
    cache->past->recent = cache->recent;
}

/* search - find the node with target uri from the head */
node_c *search(cache_list *c_list, char *uri){
    node_c *ptr = c_list->head;
    
    if(ptr == NULL) return NULL;
    while(1){
        if(!strcasecmp(ptr->c_key, uri)) return ptr;
        if(ptr == c_list->tail) return NULL;
        ptr = ptr->past;
    }
}
