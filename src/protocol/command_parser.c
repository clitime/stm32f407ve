#include "command_parser.h"
#include "version.h"
#include <stddef.h>
#include <string.h>


enum {
    CMD_IDLE_DATA = 0x00,
    CMD_ERROR_CMD = 0x01,
    CMD_GET_INFO = 0x70
};


static struct packet transmit_packet = {0};
static uint8_t buf[PACKET_DATA_LEN] = {0};

typedef struct packet *cmd_fn(const uint8_t *, uint8_t);

static cmd_fn idle_data;
static cmd_fn get_info;
static cmd_fn error_cmd;


struct packet * cmd_parser(struct packet *packet) {
    cmd_fn *return_fn = NULL;
    transmit_packet = (struct packet){0};
    switch(packet->cmd) {
    case CMD_IDLE_DATA:
        return_fn = idle_data;
        break;
    case CMD_GET_INFO:
        return_fn = get_info;
        break;
    default:
        return_fn = error_cmd;
        break;
    }
    if (!return_fn) {
        return NULL;
    }
    return return_fn(packet->data, packet->len);
}


static struct packet *idle_data(const uint8_t *d, uint8_t l) {
    (void)d;
    (void)l;
    return &transmit_packet;
}


static struct packet *error_cmd(const uint8_t *d, uint8_t l) {
    (void)d;
    (void)l;

    transmit_packet.cmd = CMD_ERROR_CMD | 0x80;
    transmit_packet.len = 0;

    return &transmit_packet;
}


static struct packet *get_info(const uint8_t *d, uint8_t l) {
    (void)d;
    (void)l;
    uint8_t ix = 0;

    const char *chb = get_device_name();
    uint8_t chb_l = strlen(chb);
    buf[ix++] = chb_l;
    memcpy(&buf[ix], chb, chb_l);
    ix += chb_l;

    chb = get_device_version();
    chb_l = strlen(chb);
    buf[ix++] = chb_l;
    memcpy(&buf[ix], chb, chb_l);
    ix += chb_l;


    chb = get_device_type();
    chb_l = strlen(chb);
    buf[ix++] = chb_l;
    memcpy(&buf[ix], chb, chb_l);
    ix += chb_l;

    transmit_packet.cmd = CMD_GET_INFO | 0x80;
    memcpy(transmit_packet.data, buf, ix);
    transmit_packet.len = ix;
    return &transmit_packet;
}
