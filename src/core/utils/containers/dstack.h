#pragma once

#include "core/utils/defines.h"

typedef struct dstack* dstack_t;
typedef struct dstack_mem_allocator
{
    void* (*allocate)(u64);
    void* (*reallocate)(void*, u64);
    void  (*free)(void*);
} dstack_mem_allocator;

dstack_t dstack_create(u64 initial_capacity, u32 item_size, dstack_mem_allocator* allocator);
void     dstack_destroy(dstack_t dstack);
bool     dstack_is_empty(dstack_t dstack);
void     dstack_push(dstack_t dstack, void* item);
void     dstack_pop(dstack_t dstack, void* out_item);
void     dstack_peek(dstack_t dstack, void* out_item);
void     dstack_clear(dstack_t dstack);
u64      dstack_count(dstack_t dstack);

#if defined(DSTACK_IMPLEMENTATION)

#define DSTACK_DEFAULT_ALLOCATOR (dstack_mem_allocator){ .allocate = malloc, .reallocate = realloc, .free = free }

typedef struct dstack {
    u32 item_size;
    dstack_mem_allocator allocator;
    u64 count, capacity;
    void* block;
} * dstack_t;

dstack_t dstack_create(u64 initial_capacity, u32 item_size, dstack_mem_allocator* _allocator)
{
    dstack_mem_allocator allocator = _allocator == NULL ? DSTACK_DEFAULT_ALLOCATOR : *_allocator;
    dstack_t dstack = (dstack_t)allocator.allocate(sizeof(struct dstack));

    dstack->allocator = allocator;
    dstack->block     = allocator.allocate(initial_capacity * item_size);
    dstack->item_size = item_size;
    dstack->capacity  = initial_capacity;
    dstack->count     = 0;

    return dstack;
}

void dstack_destroy(dstack_t dstack)
{
    if (dstack)
    {
        if (dstack->block) dstack->allocator.free(dstack->block);
        dstack->allocator.free(dstack);
    }
}

bool dstack_is_empty(dstack_t dstack)
{
    return dstack != NULL && dstack->count == 0;
}

void dstack_push(dstack_t dstack, void* item)
{
    if (!dstack) return;
    if (dstack->count >= dstack->capacity)
    {
        dstack->capacity = dstack->capacity > 0 ? dstack->capacity * 2 : 64;
        void* b = dstack->allocator.reallocate(dstack->block, dstack->capacity * dstack->item_size);
        assert(b);
        dstack->block = b;
    }
    memcpy((u8*)dstack->block + dstack->count * dstack->item_size, item, dstack->item_size);
    dstack->count++;
}

void dstack_pop(dstack_t dstack, void* out_item)
{
    if (!dstack || dstack->count == 0 || !out_item) return;
    memcpy(out_item, (u8*)dstack->block + (dstack->count - 1) * dstack->item_size, dstack->item_size);
    dstack->count--;
}

void dstack_peek(dstack_t dstack, void* out_item)
{
    if (!dstack || dstack->count == 0 || !out_item) return;
    memcpy(out_item, (u8*)dstack->block + (dstack->count - 1) * dstack->item_size, dstack->item_size);
}

void dstack_clear(dstack_t dstack)
{
    if (dstack) dstack->count = 0;
}

u64 dstack_count(dstack_t dstack)
{
    return dstack ? dstack->count : 0;
}

#endif
