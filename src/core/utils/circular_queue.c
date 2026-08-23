#include "circular_queue.h"

typedef struct circular_queue_t
{
    void *queue;
    u32 type_size;
    u32 queue_size;
    u32 num_of_elements;
    u32 tail_index;
    u32 head_index;
} circular_queue_t;

circular_queue_t *cqueue_create(u32 type_size, u32 queue_size)
{
    circular_queue_t *cq = (circular_queue_t *)malloc(sizeof(circular_queue_t));
    void *Cqueue = (void *)malloc(type_size * queue_size);
    assert(cq != NULL && Cqueue != NULL);
    cq->queue = Cqueue;
    cq->num_of_elements = 0;
    cq->queue_size = queue_size;
    cq->type_size = type_size;
    cq->tail_index = 0;
    cq->head_index = 0;
    return cq;
}

bool cqueue_enqueue(circular_queue_t *cq, void *element)
{
    assert(cq != NULL && element != NULL);
    if (cq->num_of_elements == cq->queue_size)
    {
        LOG_WARNING("try to enqueue to full queue");
        return false;
    }
    memcpy(cq->queue + (cq->tail_index * cq->type_size), element, cq->type_size);
    cq->tail_index = (cq->tail_index + 1) % (cq->queue_size);
    cq->num_of_elements += 1;
    return true;
}

bool cqueue_dequeue(circular_queue_t *cq, void *element)
{
    assert(element != NULL && element != NULL);
    if (cq->num_of_elements == 0)
    {
        LOG_WARNING("try to dequeue from empty queue");
        return false;
    }
    memcpy(element, cq->queue + (cq->head_index * cq->type_size), cq->type_size);
    cq->head_index = (cq->head_index + 1) % cq->queue_size;
    cq->num_of_elements -= 1;
    return true;
}

bool cqueue_peek(circular_queue_t *cq, void *element)
{
    
    assert(element != NULL && element != NULL);
    memcpy(element, cq->queue + (cq->head_index * cq->type_size), cq->type_size);
    return true;
}

bool cqueue_destroy(circular_queue_t *cq)
{
    if (cq)
    {
        if (cq->queue) free(cq->queue);
        free(cq);
        return true;
    }
    return false;
}

u32 cqueue_count(circular_queue_t *cq)
{
    assert(cq != NULL);
    return cq->num_of_elements;
}
