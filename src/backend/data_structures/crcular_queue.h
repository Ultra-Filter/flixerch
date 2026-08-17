#include "core/utils/defines.h"

#ifndef CRCULAR_QUEUE_H
#define CRCULAR_QUEUE_H

typedef struct qeueu
{
  void * queu;
  void * taile;
  void * head;
  u64 type_size;
  u32 queue_size;
  u32 num_of_elements;
  u32 taile_index;
  u32 head_index;
} crcular_queue ;

crcular_queue * initCqueue(u64 type_size,u32 queue_size);
void enqueueCqueue(crcular_queue * cq , void * element);
void * dequeueCqueue(crcular_queue * cq);
void * peekCqueue(crcular_queue * cq);
void destroyCqueue(crcular_queue * cq);

#endif
