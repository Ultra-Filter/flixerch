#include "crcular_queue.h"

crcular_queue * initCqueue(u64 type_size,u32 queue_size){
    crcular_queue * cq = (crcular_queue *)malloc(sizeof(crcular_queue));
    void * Cqueue =(void *) malloc(type_size * queue_size);
    cq->queu=Cqueue;
    cq->num_of_elements=0;
    cq->head=Cqueue;
    cq->taile=Cqueue;
    cq->queue_size=queue_size;
    cq->type_size=type_size;
    cq->taile_index=0;
    cq->head_index=0;
    return cq;
}

void enqueueCqueue(crcular_queue * cq,void * element){
    if(cq->num_of_elements==cq->queue_size){
       LOG_WARNING("try to enqueue to full queue");
       return;
    }
    if(cq->num_of_elements==0){
        memcpy(cq->taile,element,cq->type_size);
        cq->num_of_elements+=1;
        return;
    }
    if(cq->taile_index+1==cq->queue_size){
        cq->taile=cq->taile-cq->taile_index * cq->type_size;
        memcpy(cq->taile,element,cq->type_size);
        cq->taile_index=0;
        cq->num_of_elements+=1;
        return;
    }
    cq->taile=cq->taile+cq->type_size;
    memcpy(cq->taile,element,cq->type_size);
    cq->taile_index+=1;
    cq->num_of_elements+=1;


}

void * dequeueCqueue(crcular_queue * cq){
    void * element=malloc(cq->type_size);
    if(cq->queue_size==0){
        LOG_WARNING("try to dequeue from empty queue");
        return NULL;
    }
    if(cq->num_of_elements==1){
        memcpy(element,cq->head,cq->type_size);
        cq->num_of_elements=0;
        return element;

    }
    if(cq->head_index+1==cq->queue_size){
        memcpy(element,cq->head,cq->type_size);
        cq->head=cq->head-cq->head_index * cq->type_size;
        cq->head_index=0;
        cq->num_of_elements-=1;
        return element;
    }
    memcpy(element,cq->head,cq->type_size);
    cq->head=cq->head+cq->type_size;
    cq->head_index+=1;
    cq->num_of_elements-=1;
    return element;
}

void * peekCqueue(crcular_queue * cq){
    void * element=malloc(cq->type_size);
    memcpy(element,cq->head,cq->type_size);
    return element;
}

void destroyCqueue(crcular_queue * cq){
    free(cq->queu);
    free(cq);
}

