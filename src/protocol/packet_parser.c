#include "packet_parser.h"
#include <stddef.h>
#include <string.h>
#include <stdio.h>

static addr_t addr = 0x01;
static struct packet packet = {0};
const struct packet def_packet = {0};


void set_addres(addr_t a) {
    if (a == 0xff || a == 0x00)
        addr = 0x01;
    else
        addr = a;
}


struct packet * parse_recieve(uint8_t *d, uint8_t len) {
    if (len < DATA_POS) {
        return NULL;
    }
    packet = def_packet;

#ifdef ADDR_TWO_BYTE
    packet.addr = d[ADDR_POS] | (d[ADDR_POS + 1] << 8);
#else
    packet.addr = d[ADDR_POS];
#endif
    if (packet.addr != addr && packet.addr != BROADCAST_ADDR) {
        return NULL;
    }

    if (packet.addr == BROADCAST_ADDR) {
        packet.addr = addr;
    }

    packet.id = d[ID_POS];
    packet.sqn = d[SQN_POS];
    packet.cmd = d[CMD_POS];
    packet.len = d[LEN_POS];

    if (len - (LEN_POS + LEN_SIZE) != packet.len) {
        return NULL;
    }

    if (packet.len > PACKET_DATA_LEN) {
        return NULL;
    }

    if (packet.len > 0) {
        memcpy(packet.data, &d[DATA_POS], packet.len);
    }

    return &packet;
}
