#ifndef PACKETI_H_
#define PACKETI_H_

#include <stdint.h>

#define ADDR_TWO_BYTE

#ifndef addr_t
#ifdef ADDR_TWO_BYTE
    typedef uint16_t addr_t;
#else
    typedef uint8_t addr_t;
#endif
#endif


enum {
#ifdef ADDR_TWO_BYTE
    BROADCAST_ADDR = 0xffff
#else
    BROADCAST_ADDR = 0xff
#endif
};


#define ADDR_SIZE (sizeof(addr_t))
#define ID_SIZE (sizeof packet.id)
#define SQN_SIZE (sizeof packet.sqn)
#define CMD_SIZE (sizeof packet.cmd)
#define LEN_SIZE (sizeof packet.len)
#define ADDR_POS 0U
#define ID_POS (uint32_t)(ADDR_POS + ADDR_SIZE)
#define SQN_POS (uint32_t)(ID_POS + ID_SIZE)
#define CMD_POS (uint32_t)(SQN_POS + SQN_SIZE)
#define LEN_POS (uint32_t)(CMD_POS + CMD_SIZE)
#define DATA_POS (uint32_t)(LEN_POS + LEN_SIZE)


#define PACKET_DATA_LEN 64
struct packet {
    addr_t addr;
    uint8_t id;
    uint8_t sqn;
    uint8_t cmd;
    uint8_t len;
    uint8_t data[PACKET_DATA_LEN];
};


#endif /* PACKETI_H_ */
