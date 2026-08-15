#pragma once

#include "defines.h"

// ====================================================
//                 Ring Queue
// ====================================================

typedef struct ring_queue_s ring_queue_s;

#define new_ring_queue(cpapcity, type) ring_quque_create(capacity, sizeof(type))

ring_queue_s * ring_quque_create(u64 capacity, u32 element_size);
void ring_queue_destroy(ring_queue_s *queue);
bool ring_queue_push(ring_queue_s* queue, const void* data);
bool ring_queue_pop(ring_queue_s* queue, void* out_data);
bool ring_queue_peek(const ring_queue_s* queue, void* out_peek_data);

// ====================================================
//                 Dynamic Queue
// ====================================================
typedef void* darray_t;

#define DARRAY_DEFAULT_INITIAL_CAPACITY 256
#define new_darray(type) darray_create_empty(DARRAY_DEFAULT_INITIAL_CAPACITY, sizeof(type))

darray_t darray_create_empty(u64 initial_capacity, u64 type_size_in_bytes);
darray_t darray_create_from_carray(const void * carray, u64 length, u64 type_size_in_bytes);
darray_t darray_create_from_darray(const darray_t darray);
void   darray_insert_at(darray_t* darray, const void * element, u64 index);
void   darray_remove_at(darray_t* darray, u64 index);
void   darray_remove(darray_t* darray, const void * element);
void   darray_push(darray_t* darray, const void * element);
void   darray_pop(darray_t* darray, void* out_element);
u64    darray_get_length(const darray_t darray);
u64    darray_get_capacity(const darray_t darray);

