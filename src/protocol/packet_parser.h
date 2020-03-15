#ifndef PACKET_PARSER_H_
#define PACKET_PARSER_H_


#include "packeti.h"
#include <stdbool.h>
#include <stdint.h>


void set_addres(addr_t a);
struct packet * parse_recieve(uint8_t *d, uint8_t len);


#endif
