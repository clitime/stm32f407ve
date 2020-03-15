#ifndef UART_QUEUE_MANAGER_H_
#define UART_QUEUE_MANAGER_H_

#include <stdint.h>
#include <stdbool.h>

enum {
    QUEUE_SIZE = 64,
};
typedef void observer_fn(uint8_t d);

void queue_init(void);
void queue_append(uint8_t d);

bool registry_observer(observer_fn *fn);
void notify_observers(void);


#endif
