#ifndef RECIEVER_H_
#define RECIEVER_H_

#include <stdint.h>
#include <stdbool.h>

void transport_update(uint8_t d);

typedef void transmit_fn(const uint8_t*, uint32_t);
void transport_init(transmit_fn *fn2);


#endif
