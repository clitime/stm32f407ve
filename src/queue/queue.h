#ifndef QUEUE_H_
#define QUEUE_H_


#include <stdint.h>
#include <stdbool.h>


struct fifo;

struct fifo *fifo_create(uint8_t *b, uint32_t cap);
void fifo_delete(struct fifo *f);
void fifo_clear(struct fifo *f);
bool fifo_append(struct fifo *f, uint8_t d);
bool fifo_pop(struct fifo *f, uint8_t *d);
bool fifo_is_empty(struct fifo *f);


#endif
