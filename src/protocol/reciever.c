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
    frame_size = sizeof(struct packet),
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
    uint8_t tmp_buf[sizeof(struct packet)] = {0};
    uint8_t ix = 0;
#ifdef ADDR_TWO_BYTE
    tmp_buf[ix++] = (uint8_t)(recv_packet->addr & 0xff);
    tmp_buf[ix++] = (uint8_t)((recv_packet->addr >> 8) & 0xff);
#else
    tmp_buf[ix++] = recv_packet->addr;
#endif

    tmp_buf[ix++] = recv_packet->id;
    tmp_buf[ix++] = recv_packet->sqn;
    tmp_buf[ix++] = packet->cmd;
    tmp_buf[ix++] = packet->len;

    if (packet->len > 0) {
        for (uint8_t i = 0; i != packet->len; i++) {
            tmp_buf[ix++] = packet->data[i];
        }
    }

    uint16_t crc = calcCrc16CCITT(tmp_buf, ix);
    uint8_t crc_byte = ((crc & 0xff00) >> 8);
    tmp_buf[ix++] = crc_byte;
    crc_byte = crc & 0xff;
    tmp_buf[ix++] = crc_byte;

    uint8_t trmt_buf[sizeof(struct packet) * 2 + 2] = {0};
    uint8_t ix2 = 0;
    trmt_buf[ix2++] = bs_start;

    for (uint8_t i = 0; i != ix; i++) {
        if (tmp_buf[i] >= bs_start) {
            trmt_buf[ix2++] = bs_stuffing;
            trmt_buf[ix2++] = bs_stuffing - tmp_buf[i];
        } else {
            trmt_buf[ix2++] = tmp_buf[i];
        }
    }

    trmt_buf[ix2++] = bs_stop;

    if (trans) {
        trans(trmt_buf, ix2);
    }
}
