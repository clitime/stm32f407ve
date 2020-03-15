#ifndef RING_QUEUE_H_
#define RING_QUEUE_H_


#include <stdint.h>
#include <stdbool.h>


#ifndef RFIFO_QUANT
    enum {
        RFIFO_QUANT = 1
    };
#endif

typedef uint16_t index_t;

struct rfifo;

struct rfifo *rfifo_create(uint8_t *b, index_t cap);
void rfifo_delete(struct rfifo *f);
void rfifo_clear(struct rfifo *f);
bool rfifo_append(struct rfifo *f, uint8_t d);
bool rfifo_pop(struct rfifo *f, uint8_t *d);
bool rfifo_is_empty(struct rfifo *f);
bool rfifo_is_full(struct rfifo *f);
index_t rfifo_get_length(struct rfifo *f);


#endif
