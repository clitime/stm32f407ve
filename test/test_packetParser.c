#include "unity.h"
#include "packet_parser.h"

const addr_t test_addr = 1;

void setUp(void) {}
void tearDown(void) {}

void suiteSetUp(void) {
    set_addres(test_addr);
}

int suiteTearDown(int num_failures) {}


void test_simplePacket(void) {
#ifndef ADDR_TWO_BYTE
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x00};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x00};
#endif
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x00};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_incorrectAddress(void) {
#ifndef ADDR_TWO_BYTE
    uint8_t test_packet[] = {0x02, 0x02, 0x03, 0x04, 0x00};
#else
    uint8_t test_packet[] = {0x02, 0x00, 0x02, 0x03, 0x04, 0x00};
#endif
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_broadcastPacket(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0xff, 0x02, 0x03, 0x04, 0x00};
#else
    uint8_t test_packet[] = {0xff, 0xff, 0x02, 0x03, 0x04, 0x00};
#endif
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = test_addr, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x00};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_incorrectLenghtEmptyData(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x02};
#endif
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02, 0xaa};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x02, 0xaa};
#endif
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength2(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02, 0xaa, 0xab, 0xac};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x02, 0xaa, 0xab, 0xac};
#endif
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength3(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x00, 0xaa, 0xab, 0xac};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x00, 0xaa, 0xab, 0xac};
#endif
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_correctLength(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x03, 0xaa, 0xab, 0xac};
#else
    uint8_t test_packet[] = {0x01, 0x00, 0x02, 0x03, 0x04, 0x03, 0xaa, 0xab, 0xac};
#endif
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x03, .data = {0xaa, 0xab, 0xac}};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_fullData(void) {
#if BROADCAST_ADDR == 0xff
#define offset 5
    uint8_t test_packet[PACKET_DATA_LEN + offset] = {0x01, 0x02, 0x03, 0x04, PACKET_DATA_LEN,};
#else
#define offset 6
    uint8_t test_packet[PACKET_DATA_LEN + offset] = {0x01, 0x00, 0x02, 0x03, 0x04, PACKET_DATA_LEN,};
#endif
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        test_packet[offset + ix] = ix;
    }
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=PACKET_DATA_LEN};
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        expected.data[ix] = ix;
    }
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}



void test_overflowData(void) {
#if BROADCAST_ADDR == 0xff
    uint8_t test_packet[PACKET_DATA_LEN + 6] = {0x01, 0x02, 0x03, 0x04, PACKET_DATA_LEN + 1};
#else
    uint8_t test_packet[PACKET_DATA_LEN + 7] = {0x01, 0x00, 0x02, 0x03, 0x04, PACKET_DATA_LEN + 1};
#endif
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        test_packet[5 + ix] = ix;
    }
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}
