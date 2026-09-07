#pragma once

#include "core/utils/defines.h"
typedef struct dlinked_list_memory_allocator
{
    void * (*allocate)(u64);
    void * (*reallocate)(void*, u64);
    void   (*free)(void*);
} dlinked_list_memory_allocator;

typedef struct dlinked_list_iterator {
    struct dlinked_list_entry* entry;
    bool init;
}  dlinked_list_iter;


typedef struct dlinked_list * dlinked_list_t;

dlinked_list_t dlinked_list_create    (u64 item_size_in_bytes, dlinked_list_memory_allocator* allocator); // Allocating new list, allocator can be NULL in order to use std - malloc
void           dlinked_list_push_back (dlinked_list_t list, void* item); // pushes an item to end of list
void           dlinked_list_push_front(dlinked_list_t list, void* item); // pushes an item to front of list
void           dlinked_list_destroy   (dlinked_list_t list); // free the list items and list itself
bool           dlinked_list_pop_front (dlinked_list_t list, void* out_item); // pop front element, return true <=> list.count > 0
bool           dlinked_list_pop_back  (dlinked_list_t list, void* out_item); // pop back element, return true <=> list.count > 0
bool           dlinked_list_pop_at    (dlinked_list_t list, u32 index, void* out_item); // pop elemnt from list[index], return true <=> list.count > index.
bool           dlinked_list_get       (dlinked_list_t list, u32 index, void* out_item); // return list[index], return true <=> list.count > index [No delete]
bool           dlinked_list_get_head  (dlinked_list_t list, void* out_item); // return list.head, return true <=> list.count > 0 [No delete]
bool           dlinked_list_get_tail  (dlinked_list_t list, void* out_item); // return list.tail, return true <=> list.count > 0 [No delete]
u32            dlinked_list_push_at   (dlinked_list_t list, u32 index, void*item); // push item at list[index], return the actual index in which it insert the item [MIN(index, list.count)]
u32            dlinked_list_len       (dlinked_list_t list); // returns list.count [Num of elements in the list]

bool dlinked_list_iterator_next(dlinked_list_t list, dlinked_list_iter* iter, void* out_val); 
bool dlinked_list_iterator_next_ptr(dlinked_list_t list, dlinked_list_iter* iter, void** out_val); 

#if defined(DLINKED_LIST_IMPLEMENTATION)

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define DLINKED_LIST_DEFAULT_ALLOCATOR (dlinked_list_memory_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }

typedef struct dlinked_list_entry
{
    void * value;
    struct dlinked_list_entry * prev; 
    struct dlinked_list_entry * next; 
} dlinked_list_entry;
typedef struct dlinked_list
{
    u32 count;
    u32 item_size_in_bytes;
    dlinked_list_memory_allocator allocator;
    dlinked_list_entry * head;
    dlinked_list_entry * tail;
} *dlinked_list_t; 


dlinked_list_t dlinked_list_create(u64 item_size_in_bytes, dlinked_list_memory_allocator* allocator)
{
    dlinked_list_memory_allocator _allocator = allocator == NULL ? DLINKED_LIST_DEFAULT_ALLOCATOR : *allocator;

    dlinked_list_t list = _allocator.allocate(sizeof(struct dlinked_list));
    
    list->allocator          = _allocator;
    list->count              = 0;
    list->item_size_in_bytes = item_size_in_bytes;
    list->head               = NULL;
    list->tail               = NULL;

    return list;
}

void dlinked_list_push_back(dlinked_list_t list, void *item)
{
    assert(list && item);
    dlinked_list_entry* e = list->allocator.allocate(sizeof(dlinked_list_entry));
    e->prev = list->tail;
    if (e->prev) e->prev->next = e;
    e->next = NULL;
    list->tail = e;
    if (!list->head) list->head = e;
    e->value = list->allocator.allocate(list->item_size_in_bytes);
    memcpy(e->value, item, list->item_size_in_bytes);
    list->count++;
}

void dlinked_list_push_front(dlinked_list_t list, void *item)
{
    assert(list && item);
    dlinked_list_entry* e = list->allocator.allocate(sizeof(dlinked_list_entry));
    e->next = list->head;
    if (list->head) list->head->prev = e;
    list->head = e;
    if (!list->tail) list->tail = e;
    e->prev = NULL;
    e->value = list->allocator.allocate(list->item_size_in_bytes);
    memcpy(e->value, item, list->item_size_in_bytes);
    list->count++;
}

void dlinked_list_destroy(dlinked_list_t list)
{
    while (dlinked_list_pop_front(list, NULL));
    list->allocator.free(list);
}

bool dlinked_list_pop_front(dlinked_list_t list, void *out_item)
{
    assert(list);

    if (list->head)
    {
        dlinked_list_entry* e = list->head;
        if (out_item) memcpy(out_item, e->value, list->item_size_in_bytes);
        if (e->next) e->next->prev = NULL;
        list->head = e->next;
        list->allocator.free(e->value);
        list->allocator.free(e);
        list->count--;
        if (list->count == 0) list->tail = NULL;
        return true;
    }
    
    return false;
}

bool dlinked_list_pop_back(dlinked_list_t list, void *out_item)
{
    assert(list);
    
    if (list->tail)
    {
        dlinked_list_entry* e = list->tail;
        if (out_item) memcpy(out_item, e->value, list->item_size_in_bytes);
        e->prev->next = NULL;
        list->tail = e->prev;
        list->allocator.free(e->value);
        list->allocator.free(e);
        list->count--;
        if (list->count == 0) list->head = NULL;
        return true;
    }

    return false;
}

bool dlinked_list_pop_at(dlinked_list_t list, u32 index, void *out_item)
{
    bool ret_val = index < list->count;
    
    if (ret_val)
    {
        dlinked_list_entry* e = list->head;
        for (u32 i = 0; e && i < index; i++) e = e->next;
        assert(e); // Should never fail
        dlinked_list_entry* pe = e->prev;
        dlinked_list_entry* ne = e->next;
        if (pe) pe->next = ne;
        if (ne) ne->prev = pe;
        if (out_item) memcpy(out_item, e->value, list->item_size_in_bytes);
        list->allocator.free(e->value);
        list->allocator.free(e);
        list->count--;
    }

    return ret_val;
}

bool dlinked_list_get(dlinked_list_t list, u32 index, void *out_item)
{
    bool ret_val = index < list->count;
    
    if (ret_val)
    {
        dlinked_list_entry* e = list->head;
        for (u32 i = 0; e && i < index; i++) e = e->next;
        assert(e); // Should never fail
        if (out_item) memcpy(out_item, e->value, list->item_size_in_bytes);
    }

    return ret_val;
}

bool dlinked_list_get_head(dlinked_list_t list, void *out_item)
{
    if (list->head)
    {
        if (out_item) memcpy(out_item, list->head->value, list->item_size_in_bytes);
        return true;
    }
    return false;
}

bool dlinked_list_get_tail(dlinked_list_t list, void *out_item)
{
    if (list->tail)
    {
        if (out_item) memcpy(out_item, list->tail->value, list->item_size_in_bytes);
        return true;
    }
    return false;
}

u32 dlinked_list_push_at(dlinked_list_t list, u32 index, void *item)
{
    assert(list && item);

    u32 idx = index < list->count ? index : list->count;

    if (idx == 0)
    {
        dlinked_list_push_front(list, item);
        return idx;    
    }
    
    dlinked_list_entry* e = list->head;
    for (u32 i = 0; e && i < idx; i++) e = e->next;
    assert(e); // Should never be NULL

    dlinked_list_entry* ne = list->allocator.allocate(sizeof(dlinked_list_entry));
    ne->value = list->allocator.allocate(list->item_size_in_bytes);
    memcpy(ne->value, item, list->item_size_in_bytes);

    ne->next = e;
    ne->prev = e->prev;
    e->prev->next = ne;
    e->prev = ne;

    list->count++;
    
    return idx;
}

inline u32 dlinked_list_len(dlinked_list_t list)
{
    return list->count;
} 

bool dlinked_list_iterator_next(dlinked_list_t list, dlinked_list_iter* iter, void* out_val)
{
    if (!iter->entry && !iter->init) 
    { 
        if (!list) return false;
        iter->entry = list->head;
        iter->init = true;
    }
    if (!iter->entry) return false;

    if (out_val) memcpy(out_val, iter->entry->value, list->item_size_in_bytes);
    iter->entry = iter->entry->next;
    return true;
}

bool dlinked_list_iterator_next_ptr(dlinked_list_t list, dlinked_list_iter* iter, void** out_val)
{
    if (!iter->entry && !iter->init) 
    { 
        iter->entry = list->head;
        iter->init = true;
    }
    if (!iter->entry) return false;
    if (out_val) *out_val = iter->entry->value;
    iter->entry = iter->entry->next;
    return true;
}

#endif