#include "data_structures.h"

// ====================================================
//                 Ring Queue
// ====================================================

typedef struct ring_queue_s {
    u32 element_size;
    u64 capacity;
    s64 head, tail; // offsets
    u8* block;
} ring_queue_s;

ring_queue_s * ring_quque_create(u64 capacity, u32 element_size)
{
    ring_queue_s * queue = malloc(sizeof(ring_queue_s));
    assert(queue != NULL);

    queue->element_size = element_size;
    queue->capacity     = capacity;
    queue->head         = -1;
    queue->block        = malloc(capacity * element_size);
    assert(queue->block != NULL);
    queue->tail         = 0;
    
    return queue;
}

void ring_queue_destroy(ring_queue_s *queue)
{
    if (queue)
    {
        if (queue->block)
        {
            free(queue->block);
        }
        free(queue);
    }
}

bool ring_queue_push(ring_queue_s* queue, const void* data)
{
    assert(queue != NULL && data != NULL);

    if(queue->tail == queue->head)
    {
        return false; // Queue is full
    }

    memcpy(queue->block + queue->tail * queue->element_size, data, queue->element_size);

    queue->tail = (queue->tail + 1) % queue->capacity;

    if (queue->head == -1)
    {
        queue->head = 0;
    }

    return true;
}

bool ring_queue_pop(ring_queue_s* queue, void* out_data)
{
    assert(queue != NULL && out_data != NULL);

    if (queue->head == -1)
    {
        return false;
    }

    memcpy(out_data, queue->block + queue->head * queue->element_size, queue->element_size);

    queue->head = (queue->head + 1) % queue->capacity;

    if (queue->head == queue->tail)
    {
        queue->head = -1;
    }

    return true;
}

bool ring_queue_peek(const ring_queue_s* queue, void* out_peek_data)
{
    assert(queue != NULL && out_peek_data != NULL);

    if (queue->head == -1)
    {
        return false;
    }

    memcpy(out_peek_data, queue->block + queue->head * queue->element_size, queue->element_size);
    return true;
}

u64 ring_queue_count(const ring_queue_s *queue)
{
    s64 c = queue->head == -1 ? 0 : queue->tail - queue->head;
        c = c >= 0 ? c : (s64)queue->capacity + c;
    return (u64)c;
}

// ====================================================
//                 Dynamic Array
// ====================================================

#define _head_from_block(block) (((_darray_header*)block)-1)
#define _block_from_head(head) (void*)(head + 1)

typedef struct _darray_header
{
    u32 type_size_in_bytes;
    u64 capacity;
    u64 count;
} _darray_header;

darray_t darray_create_empty(u64 initial_capacity, u32 type_size_in_bytes)
{
    _darray_header* header = malloc(sizeof(_darray_header) + initial_capacity * type_size_in_bytes);
    
    header->capacity           = initial_capacity;
    header->count              = 0;
    header->type_size_in_bytes = type_size_in_bytes;
    
    return _block_from_head(header);
}

darray_t darray_create_from_carray(const void *carray, u64 length, u64 type_size_in_bytes)
{
    _darray_header* header = malloc(sizeof(_darray_header) + length * type_size_in_bytes);
    
    header->capacity           = length;
    header->count              = length;
    header->type_size_in_bytes = type_size_in_bytes;
    
    void* block = _block_from_head(header);
    memcpy(block, carray, length * type_size_in_bytes);
    
    return block;
}

darray_t darray_create_from_darray(const darray_t darray)
{
    _darray_header* old_header = _head_from_block(darray);
    _darray_header* header = malloc(sizeof(_darray_header) + old_header->capacity * old_header->type_size_in_bytes);

    memcpy(header, old_header, sizeof(_darray_header) + old_header->capacity * old_header->type_size_in_bytes);

    return _block_from_head(header);
}

void darray_insert_at(darray_t* darray, const void *element, u64 index)
{
    if (darray == NULL) return; // Silence error
    
    _darray_header * head = _head_from_block((*darray));

    if (head->count <= index) return; // Silence error
    
    if (head->capacity <= head->count + 1)
    {
        head->capacity *= 2;

        void* reallocation = realloc(head, sizeof(_darray_header) + head->capacity * head->type_size_in_bytes);
        assert(reallocation != NULL);
        
        head = reallocation;
        *darray = _block_from_head(head);
    }

    void * dest = (*darray) + (index + 1) * head->type_size_in_bytes;
    void * src  = (*darray) +  index      * head->type_size_in_bytes;
    u64 amount  = (head->count - index)   * head->type_size_in_bytes;

    memmove(dest, src, amount);
    memcpy(src, element, head->type_size_in_bytes);

    head->count++;
}

void darray_remove_at(darray_t* darray, u64 index)
{
    _darray_header* head = _head_from_block((*darray));

    if (index >= head->count) return; // Silence error

    memmove(darray + index, darray + index + 1, (head->count - index - 1) *  head->type_size_in_bytes);

    head->count--;
    // TODO: Add realloc when head->count < head->capacity / 4
    // Dont forget to reasign darray to new reallocation
}

void darray_remove(darray_t* darray, const void *element)
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

void darray_push(darray_t* darray, const void *element)
{
    if (darray == NULL) return; // Silence error
    
    _darray_header * head = _head_from_block((*darray));
    
    if (head->capacity <= head->count + 1)
    {
        head->capacity *= 2;

        void* reallocation = realloc(head, sizeof(_darray_header) + head->capacity * head->type_size_in_bytes);
        assert(reallocation != NULL);
        
        head = reallocation;
        *darray = _block_from_head(head);
    }

    void * dest = (*darray) + head->count * head->type_size_in_bytes;

    memcpy(dest, element, head->type_size_in_bytes);

    head->count++;
}

void darray_pop(darray_t* darray, void *out_element)
{
    if (out_element == NULL) return; // Silence error

    _darray_header* head = _head_from_block((*darray));


    memmove(out_element, darray + --head->count, head->type_size_in_bytes);

    head->count--;
    // TODO: Add realloc when head->count < head->capacity / 4
    // Dont forget to reasign darray to new reallocation
}

u64 darray_get_length(const darray_t darray)
{
    if (darray == NULL) return 0; // Silence error
    return _head_from_block(darray)->count;
}

u64 darray_get_capacity(const darray_t darray)
{
    if (darray == NULL) return 0; // Silence error
    return _head_from_block(darray)->capacity;
}

