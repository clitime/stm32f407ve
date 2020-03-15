#include "uart_queue_manager.h"
#include "ring_queue.h"
#include <stddef.h>

static struct rfifo *queue;
static uint8_t buffer[QUEUE_SIZE];

enum {
    OBSERVERS_SIZE = 1
};

static observer_fn *observers[OBSERVERS_SIZE] = { 0 };

void queue_init(void) {
    queue = rfifo_create(buffer, QUEUE_SIZE);
}

void queue_append(uint8_t d) {
    rfifo_append(queue, d);
}

bool registry_observer(observer_fn *fn) {
    uint8_t ix = 0;
    for (; ix != OBSERVERS_SIZE && observers[ix]; ix++) {
        continue;
    }

    if (ix >= OBSERVERS_SIZE) {
        return false;
    }

    observers[ix] = fn;
    return true;
}

void notify_observers(void) {
    uint8_t data = 0;
    while (!rfifo_is_empty(queue)) {
        rfifo_pop(queue, &data);
        for (uint8_t ix = 0; ix != OBSERVERS_SIZE; ix++) {
            if (observers[ix] == NULL) {
                continue;
            }
            observers[ix](data);
        }
    }
}
