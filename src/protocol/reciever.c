#include "reciever.h"
#include "crc.h"
#include <stddef.h>

#include "command_parser.h"
#include "packet_parser.h"


const uint8_t bs_start = 0xfd;
const uint8_t bs_stop = 0xfe;
const uint8_t bs_stuffing = 0xff;

const uint8_t minimal_frame_size = 7;

enum {
    frame_amount = 2,
    frame_size = 64,
};

static uint8_t frame[frame_size];
static uint8_t ix_frame = 0;
static transmit_fn *trans = NULL;

static bool parse_recv_data(uint8_t d);
static void parse_transmit(const struct packet *, const struct packet *);
static bool check_crc(void);


/**
 * initialize function for transfer
 */
void transport_init(transmit_fn *fn) {
    trans = fn;
}


void transport_update(uint8_t d) {
    if (!parse_recv_data(d)) {
        return;
    }
    struct packet *recv_msg = NULL;
    if (check_crc()) {
        recv_msg = parse_recieve(frame, ix_frame);
    }
    if (!recv_msg) {
        return;
    }

    struct packet *trmt_msg = cmd_parser(recv_msg);
    if (trmt_msg) {
        parse_transmit(trmt_msg, recv_msg);
    }
}


static bool parse_recv_data(uint8_t d) {
    static enum {st_recv, st_bytestuffing, st_stop} state = st_stop;
    enum {act_append, act_stop, act_none} action = act_none;

    if (d == bs_start) {
        ix_frame = 0;
        state = st_recv;
        return false;
    } else if (d == bs_stop) {
        if (state == st_recv) {
            state = st_stop;
            return true;
        }
        return false;
    } else if (d == bs_stuffing) {
        if (state == st_recv) {
            state = st_bytestuffing;
            return false;
        }
        action = act_stop;
    }

    switch (state) {
    case st_recv:
        action = act_append;
        break;
    case st_bytestuffing:
        state = st_recv;
        action = act_append;
        d = bs_stuffing - d;
        break;
    case st_stop:
        /* nothing */
        break;
    default:
        /* nothing */
        break;
    }

    switch(action) {
    case act_append:
        frame[ix_frame++] = d;
        if (ix_frame == frame_size) {
            state = st_stop;
        }
        break;
    case act_stop:
        state = st_stop;
        ix_frame = 0;
        break;
    case act_none:
        /* nothing */
        break;
    default:
        /* nothing */
        break;
    }
    return false;
}


static bool check_crc(void) {
    if (ix_frame < minimal_frame_size) {
        return false;
    }

    uint16_t recv_crc = frame[ix_frame - 1] | (frame[ix_frame - 2] << 8);
    ix_frame -= 2;
    uint16_t calc_crc = calcCrc16CCITT(frame, ix_frame);

    if (recv_crc == calc_crc) {
        return true;
    }
    return false;
}


static void parse_transmit(const struct packet *packet, const struct packet *recv_packet) {
    uint8_t trmt_buf[sizeof(struct packet) * 2 + 2] = {0};
    uint8_t ix = 0;
    trmt_buf[ix++] = bs_start;

#if BROADCAST_ADDR==0xffff
    trmt_buf[ix++] = (uint8_t)(recv_packet->addr & 0xff);
    trmt_buf[ix++] = (uint8_t)((recv_packet->addr >> 8) & 0xff);
#else
    if (recv_packet->addr >= bs_start) {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - recv_packet->addr;
    } else {
        trmt_buf[ix++] = recv_packet->addr;
    }
#endif

    if (recv_packet->id >= bs_start) {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - recv_packet->id;
    } else {
        trmt_buf[ix++] = recv_packet->id;
    }

    if (recv_packet->sqn >= bs_start) {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - recv_packet->sqn;
    } else {
        trmt_buf[ix++] = recv_packet->sqn;
    }

    if (packet->cmd >= bs_start) {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - packet->cmd;
    } else {
        trmt_buf[ix++] = packet->cmd;
    }

    if (packet->len >= bs_start) {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - packet->len;
    } else {
        trmt_buf[ix++] = packet->len;
    }

    if (packet->len > 0) {
        for (uint8_t i = 0; i != packet->len; i++) {
            if (packet->data[i] < bs_start) {
                trmt_buf[ix++] = packet->data[i];
            } else {
                trmt_buf[ix++] = bs_stuffing;
                trmt_buf[ix++] = bs_stuffing - packet->data[i];
            }
        }
    }

    union {
        uint8_t data[2];
        uint16_t val;
    } crc;
    crc.val = calcCrc16CCITT(trmt_buf, ix);
    if (crc.data[1] < bs_start) {
        trmt_buf[ix++] = crc.data[1];
    } else {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - crc.data[1];
    }
    if (crc.data[0] < bs_start) {
        trmt_buf[ix++] = crc.data[0];
    } else {
        trmt_buf[ix++] = bs_stuffing;
        trmt_buf[ix++] = bs_stuffing - crc.data[0];
    }

    if (trans) {
        trans(trmt_buf, ix);
    }
}
