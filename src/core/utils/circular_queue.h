#pragma once

#include "core/utils/defines.h"


typedef struct circular_queue_t circular_queue_t;

circular_queue_t * cqueue_create(u32 type_size,u32 queue_size);
bool cqueue_enqueue(circular_queue_t * cq , void * element);
bool cqueue_dequeue(circular_queue_t * cq ,void * element);
bool cqueue_peek(circular_queue_t * cq ,void * element);
bool cqueue_destroy(circular_queue_t * cq);
u32  cqueue_count(circular_queue_t * cq);
