#include "queue.h"
#include <stddef.h>


enum {
    FIFO_QUANT = 2
};


struct fifo {
    uint8_t *buffer;
    uint32_t capacity;
    uint8_t *head;
    uint8_t *tail;
    uint32_t length;
};


static struct fifo fifo[FIFO_QUANT];
static bool is_busy[FIFO_QUANT];


struct fifo *fifo_create(uint8_t *b, uint32_t cap) {
    if (b == NULL) {
        return NULL;
    }

    uint32_t ix_fifo = 0;
    for (; ix_fifo < FIFO_QUANT && is_busy[ix_fifo] == true; ix_fifo++) {
        continue;
    }

    if (ix_fifo >= FIFO_QUANT) {
        return NULL;
    }

    fifo[ix_fifo].buffer = b;
    fifo[ix_fifo].capacity = cap;

    fifo_clear(&fifo[ix_fifo]);

    is_busy[ix_fifo] = true;
    return &fifo[ix_fifo];
}


void fifo_clear(struct fifo *f) {
    f->head = f->buffer;
    f->tail = f->buffer;
    f->length = 0;
}


void fifo_delete(struct fifo *f) {
    for (uint8_t i = 0; i != FIFO_QUANT; i++) {
        if (&fifo[i] == f) {
            is_busy[i] = false;
            fifo_clear(f);
            f->capacity = 0;
            f->buffer = NULL;
        }
    }
}


bool fifo_append(struct fifo *f, uint8_t d) {
    if (f == NULL) {
        return false;
    }

    if (f->length >= f->capacity) {
        return false;
    }

    *f->tail++ = d;

    if (f->tail >= &f->buffer[f->capacity]) {
        f->tail = f->buffer;
    }

    f->length++;

    return true;
}


bool fifo_pop(struct fifo *f, uint8_t *d) {
    if (f == NULL) {
        return false;
    }

    if (f->length == 0) {
        return false;
    }

    *d = *f->head++;

    if (f->head >= &f->buffer[f->capacity]) {
        f->head = f->buffer;
    }

    f->length--;
    return true;
}


bool fifo_is_empty(struct fifo *f) {
    return !(bool)f->length;
}
