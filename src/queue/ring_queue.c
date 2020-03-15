#include "ring_queue.h"
#include <stddef.h>


struct rfifo {
    uint8_t *buffer;
    index_t capacity;
    index_t mask;
    index_t ix_r;
    index_t ix_w;
};


static struct rfifo fifo[RFIFO_QUANT];
static bool is_busy[RFIFO_QUANT];


#define assert_fifo(x)                                                             \
    do {                                                                       \
        if (!(x))                                                               \
            while (1);                                                           \
    } while (0)


struct rfifo *rfifo_create(uint8_t *b, index_t cap) {
    if (b == NULL) {
        return NULL;
    }

    assert_fifo((cap & (cap - 1)) == 0);

    uint32_t ix_fifo = 0;
    for (; ix_fifo < RFIFO_QUANT && is_busy[ix_fifo]; ix_fifo++) {
        continue;
    }

    if (ix_fifo >= RFIFO_QUANT) {
        return NULL;
    }

    fifo[ix_fifo].buffer = b;
    fifo[ix_fifo].capacity = cap;
    fifo[ix_fifo].mask = cap - 1;

    rfifo_clear(&fifo[ix_fifo]);

    is_busy[ix_fifo] = true;
    return &fifo[ix_fifo];
}


void rfifo_clear(struct rfifo *f) {
    f->ix_r = 0;
    f->ix_w = 0;
}


void rfifo_delete(struct rfifo *f) {
    for (uint8_t i = 0; i != RFIFO_QUANT; i++) {
        if (&fifo[i] == f) {
            is_busy[i] = false;
            rfifo_clear(f);
            f->capacity = 0;
            f->buffer = NULL;
        }
    }
}


bool rfifo_append(struct rfifo *f, uint8_t d) {
    if (f == NULL) {
        return false;
    }

    if (rfifo_is_full(f)) {
        return false;
    }

    f->buffer[f->ix_w++ & f->mask] = d;
    return true;
}


bool rfifo_pop(struct rfifo *f, uint8_t *d) {
    if (f == NULL) {
        return false;
    }
    if (rfifo_is_empty(f)) {
        return false;
    }

    *d = f->buffer[f->ix_r++ & f->mask];
    return true;
}


bool rfifo_is_empty(struct rfifo *f) {
    return f->ix_r == f->ix_w;
}


index_t rfifo_get_length(struct rfifo *f) {
    return (f->ix_w - f->ix_r) & f->mask;
}


bool rfifo_is_full(struct rfifo *f) {
    return (f->ix_w - f->ix_r) & ~f->mask;
}
