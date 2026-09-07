#pragma once

#include "core/utils/defines.h"

typedef struct dqueue * dqueue_t;
typedef struct dqueue_mem_allocator {
    void* (*allocate)(u64);
    void* (*reallocate)(void*, u64);
    void (*free)(void*);
} dqueue_mem_allocator;

dqueue_t dqueue_create(u32 init_capacity, u32 sizeof_element, dqueue_mem_allocator* allocator);
void dqueue_destroy(dqueue_t queue);
void dqueue_push(dqueue_t* queue, void* element);
bool dqueue_pop(dqueue_t queue, void* out_element);
bool dqueue_peek(dqueue_t queue, void* out_element);
void dqueue_clear(dqueue_t queue);
u32 dqueue_len(dqueue_t queue);

#if defined(DQUEUE_IMPLEMENTATION)

typedef struct dqueue {
    u32 capacity, length;
    u32 h, t, sizeof_element;
    dqueue_mem_allocator allocator;
} * dqueue_t;

#define DEFAULT_ALLOCATOR (dqueue_mem_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }
#define QUEUE_START(q) (u8*)((dqueue_t)q + 1)

dqueue_t dqueue_create(u32 init_capacity, u32 sizeof_element, dqueue_mem_allocator* allocator)
{
    dqueue_mem_allocator _allocator = allocator == NULL ? DEFAULT_ALLOCATOR : *allocator;
    dqueue_t queue = (dqueue_t)_allocator.allocate(sizeof(struct dqueue) + init_capacity * sizeof_element);
    assert(queue);
    
    queue->allocator = _allocator;
    queue->capacity = init_capacity;
    queue->length = 0;
    
    queue->t = 0;
    queue->h = 0;
    
    queue->sizeof_element = sizeof_element;
    
    return queue;
}

void dqueue_destroy(dqueue_t queue)
{
    if (queue) queue->allocator.free(queue);
}

void dqueue_push(dqueue_t* _queue, void* element)
{
    assert(_queue && *_queue && element);
    dqueue_t queue = *_queue;

    if (queue->length >= queue->capacity)
    {
        queue->capacity *= 2;
        void * real = queue->allocator.reallocate(queue, queue->capacity);
        assert(real);
        queue = real;
        *_queue = queue;
    }

    u8* block = QUEUE_START(queue);

    u8 * insert_pos = block + queue->t * queue->sizeof_element;
    memcpy(insert_pos, element, queue->sizeof_element);

    queue->t = (queue->t + 1) % queue->capacity;
    queue->length += 1; 
}

bool dqueue_pop(dqueue_t queue, void *out_element)
{
    assert(queue && out_element);

    if (queue->length == 0) return false;

    u8* p = QUEUE_START(queue);
    
    u8* pop_pos = p + queue->h * queue->sizeof_element;
    memcpy(out_element, pop_pos, queue->sizeof_element);
    queue->h = (queue->h + 1) % queue->capacity;
    queue->length--;

    return true;
}

bool dqueue_peek(dqueue_t queue, void *out_element)
{
    assert(queue && out_element);
    
    if (queue->length == 0) return false;
    
    u8* p = QUEUE_START(queue);
    
    u8* pop_pos = p + queue->h * queue->sizeof_element;
    memcpy(out_element, pop_pos, queue->sizeof_element);
    
    return true;
}

void dqueue_clear(dqueue_t queue)
{
    assert(queue);

    queue->length = 0;
    queue->h = 0;
    queue->t = 0;
}

inline u32 dqueue_len(dqueue_t queue)
{
    return queue ? queue->length : 0;
}

#endif