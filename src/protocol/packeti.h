#ifndef PACKETI_H_
#define PACKETI_H_

#include <stdint.h>


#ifndef addr_t
    typedef uint8_t addr_t;
#endif


#define BROADCAST_EXPR (sizeof(addr_t) == 2 ? 0xffff : 0xff)
enum {
    BROADCAST_ADDR = BROADCAST_EXPR
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
