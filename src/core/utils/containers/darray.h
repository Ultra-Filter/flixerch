#pragma once

#include "../defines.h"

typedef struct darr_memory_allocator
{
    void* (*allocate)(u64);
    void* (*reallocate)(void*, u64);
    void  (*free)(void*); 
} darr_memory_allocator;

typedef enum darr_sort_type_enum
{
    DARRAY_SORT_TYPE_MERGE_SORT, // Not implemented
    DARRAY_SORT_TYPE_QUICK_SORT,
} darr_sort_type_enum;

void * darr_create_empty(u64 initial_capacity, u64 type_size_in_bytes, darr_memory_allocator* allocator);
void * darr_create_from_carr(const void * carray, u64 length, u64 type_size_in_bytes, darr_memory_allocator* allocator);
void * darr_create_from_darr(const void * darray, darr_memory_allocator* allocator);
void   darr_insert_at(void ** darray, const void * element, u64 index);
void   darr_remove_at(void ** darray, u64 index);
void   darr_remove(void ** darray, const void * element);
void   darr_push(void** darray, const void * element);
void   darr_pop(void** darray, void* out_element);
void   darr_sort(void* darray, bool (*compare)(void*, void*), darr_sort_type_enum sort_type);
u64    darr_get_capacity(const void * darray);

#define darrlen(darr) (darr) == NULL ? 0 : _head_from_block(darray)->count

#if defined(DARRAY_IMPLEMENTATION) 

#define DARRAY_DEFAULT_ALLOCATOR (darr_memory_allocator){ .allocate = malloc, .reallocate = realloc , .free = free}
#define _head_from_block(block) (((_darray_header*)block)-1)
#define _block_from_head(head) (void*)(head + 1)

typedef struct _darray_header
{
    darr_memory_allocator allocator;
    u64 capacity;
    u64 count;
    u64 type_size_in_bytes;
} _darray_header;

static void _darr_merge_sort(void *darray, u64 start, u64 end, bool (*compare)(void *, void *));


void * darr_create_empty(u64 initial_capacity, u64 type_size_in_bytes, darr_memory_allocator* allocator)
{
    darr_memory_allocator _allocator = (allocator == NULL) ? DARRAY_DEFAULT_ALLOCATOR : *allocator;
    
    _darray_header* header = (_darray_header*)_allocator.allocate(sizeof(_darray_header) + initial_capacity * type_size_in_bytes);
    
    header->allocator          = _allocator;
    header->capacity           = initial_capacity;
    header->count              = 0;
    header->type_size_in_bytes = type_size_in_bytes;
    
    return _block_from_head(header);
}

void * darr_create_from_carr(const void *carray, u64 length, u64 type_size_in_bytes, darr_memory_allocator *allocator)
{
    darr_memory_allocator _allocator = (allocator == NULL) ? DARRAY_DEFAULT_ALLOCATOR : *allocator;
    
    _darray_header* header = (_darray_header*)_allocator.allocate(sizeof(_darray_header) + length * type_size_in_bytes);
    
    header->allocator          = _allocator;
    header->capacity           = length;
    header->count              = length;
    header->type_size_in_bytes = type_size_in_bytes;
    
    void* block = _block_from_head(header);
    memcpy(block, carray, length * type_size_in_bytes);
    
    return block;
}

void * darr_create_from_darr(const void *darray, darr_memory_allocator *allocator)
{
    darr_memory_allocator _allocator = (allocator == NULL) ? DARRAY_DEFAULT_ALLOCATOR : *allocator;

    _darray_header* old_header = _head_from_block(darray);
    _darray_header* header = (_darray_header*)_allocator.allocate(sizeof(_darray_header) + old_header->capacity * old_header->type_size_in_bytes);

    memcpy(header, old_header, sizeof(_darray_header) + old_header->capacity * old_header->type_size_in_bytes);

    return _block_from_head(header);
}

void darr_insert_at(void ** darray, const void *element, u64 index)
{
    if (darray == NULL) return; // Silence error
    
    _darray_header * head = _head_from_block((*darray));

    if (head->count <= index) return; // Silence error
    
    if (head->capacity <= head->count + 1)
    {
        head->capacity *= 2;

        void* reallocation = head->allocator.reallocate(head, sizeof(_darray_header) + head->capacity * head->type_size_in_bytes);
        assert(reallocation != NULL);
        
        head = (_darray_header*)reallocation;
        *darray = _block_from_head(head);
    }

    void * dest = (*darray) + (index + 1) * head->type_size_in_bytes;
    void * src  = (*darray) +  index      * head->type_size_in_bytes;
    u64 amount  = (head->count - index)   * head->type_size_in_bytes;

    memmove(dest, src, amount);
    memcpy(src, element, head->type_size_in_bytes);

    head->count++;
}

void darr_remove_at(void ** darray, u64 index)
{
    _darray_header* head = _head_from_block((*darray));

    if (index >= head->count) return; // Silence error

    memmove(darray + index, darray + index + 1, (head->count - index - 1) *  head->type_size_in_bytes);

    head->count--;
    // TODO: Add realloc when head->count < head->capacity / 4
    // Dont forget to reasign darray to new reallocation
}

void darr_remove(void ** darray, const void *element)
{
    if (element == NULL) return; // Silence error
    
    _darray_header* head = _head_from_block((*darray));
    
    for (u64 i = 0; i < head->count; i+= head->type_size_in_bytes)
    {
        bool found = true;
        for (u64 j = 0; j < head->type_size_in_bytes; j++)
        {
            if (*(((char*)(*darray)) + i + j) != *(((char*)element) + j))
            {
                found = false;
                break;
            }
        }
        
        if (found)
        {
            memmove(darray + i, darray + i + 1, (head->count - i - 1) *  head->type_size_in_bytes);
            head->count--;
            // TODO: Add realloc when head->count < head->capacity / 4
            // Dont forget to reasign darray to new reallocation
            return;
        }
    }
}

void darr_push(void **darray, const void *element)
{
    if (darray == NULL) return; // Silence error
    
    _darray_header * head = _head_from_block((*darray));
    
    if (head->capacity <= head->count + 1)
    {
        head->capacity *= 2;

        void* reallocation = head->allocator.reallocate(head, sizeof(_darray_header) + head->capacity * head->type_size_in_bytes);
        assert(reallocation != NULL);
        
        head = (_darray_header*)reallocation;
        *darray = _block_from_head(head);
    }

    void * dest = (*darray) + head->count * head->type_size_in_bytes;

    memcpy(dest, element, head->type_size_in_bytes);

    head->count++;
}

void darr_pop(void **darray, void *out_element)
{
    if (out_element == NULL) return; // Silence error

    _darray_header* head = _head_from_block((*darray));


    memmove(out_element, darray + --head->count, head->type_size_in_bytes);

    head->count--;
    // TODO: Add realloc when head->count < head->capacity / 4
    // Dont forget to reasign darray to new reallocation
}

inline void darr_sort(void *darray, bool (*compare)(void *, void *), darr_sort_type_enum sort_type)
{
    (void)darray; (void)compare; (void)sort_type; 
    assert(0 && "NOT IMPLEMENTED!");
    // switch (sort_type)
    // {
    //     case DARRAY_SORT_TYPE_MERGE_SORT:
    //     {
    //         _darray_header * head = _head_from_block(darray);
    //         _darray___merge_sort(darray, 0, head->count ,compare);
    //     } break;
    //     case DARRAY_SORT_TYPE_QUICK_SORT:
    //     {
    //         _darray_header * head = _head_from_block(darray);
    //         qsort(darray, head->count, head->type_size_in_bytes , compare);
    //     } break;
    //     default: { } break;
    // }
}

u64 darr_get_capacity(const void *darray)
{
    if (darray == NULL) return 0; // Silence error
    return _head_from_block(darray)->capacity;
}

static void _darr_merge_sort(void *darray, u64 start, u64 end, bool (*compare)(void *, void *))
{
    (void)darray; (void)start; (void)end; (void)compare;
    assert(0 && "NOT IMPLEMENTED");
}

#endif
