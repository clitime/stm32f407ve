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
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x00};
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x00};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_incorrectAddress(void) {
    uint8_t test_packet[] = {0x02, 0x02, 0x03, 0x04, 0x00};
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_broadcastPacket(void) {
    uint8_t test_packet[] = {0xff, 0x02, 0x03, 0x04, 0x00};
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0xff, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x00};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_incorrectLenghtEmptyData(void) {
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02};
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength(void) {
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02, 0xaa};
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength2(void) {
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x02, 0xaa, 0xab, 0xac};
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_incorrectLength3(void) {
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x00, 0xaa, 0xab, 0xac};
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}


void test_correctLength(void) {
    uint8_t test_packet[] = {0x01, 0x02, 0x03, 0x04, 0x03, 0xaa, 0xab, 0xac};
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=0x03, .data = {0xaa, 0xab, 0xac}};
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}


void test_fullData(void) {
    uint8_t test_packet[PACKET_DATA_LEN + 5] = {0x01, 0x02, 0x03, 0x04, PACKET_DATA_LEN,};
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        test_packet[5 + ix] = ix;
    }
    struct packet *act_packet = parse_recieve(test_packet, sizeof(test_packet));
    struct packet expected = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x04, .len=PACKET_DATA_LEN};
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        expected.data[ix] = ix;
    }
    TEST_ASSERT_EQUAL_MEMORY(&expected, act_packet, sizeof(struct packet));
}



void test_overflowData(void) {
    uint8_t test_packet[PACKET_DATA_LEN + 6] = {0x01, 0x02, 0x03, 0x04, PACKET_DATA_LEN + 1};
    for (uint32_t ix = 0; ix != PACKET_DATA_LEN + 1; ix++) {
        test_packet[5 + ix] = ix;
    }
    TEST_ASSERT_EQUAL_PTR(NULL, parse_recieve(test_packet, sizeof(test_packet)));
}
