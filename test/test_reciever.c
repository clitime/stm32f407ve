#include "unity.h"
#include "reciever.h"
#include <string.h>
#include "crc.h"
#include "packeti.h"
#include "packet_parser.h"
#include "command_parser.h"
#include "mock_version.h"


const addr_t test_addr = 1;

static uint8_t trans_buffer[sizeof(struct packet) * 2] = {0};
static void trans_fn(const uint8_t *data, uint32_t len) {
    memcpy(trans_buffer, data, len);
}

void setUp(void) {}
void tearDown(void) {}

void suiteSetUp(void) {
    set_addres(test_addr);
    transport_init(trans_fn);
}

int suiteTearDown(int num_failures) {}


void test_simpleTransportUpdate(void) {
    const char dev_name[] = "test device";
    get_device_name_ExpectAndReturn(dev_name);
    const char dev_ver[] = "0.0.1 (2020-03-17 aabbccddeeff8877)";
    get_device_version_ExpectAndReturn(dev_ver);
    const char dev_type[] = "test";
    get_device_type_ExpectAndReturn(dev_type);

    uint16_t crc = calcCrc16CCITT((uint8_t []){0xff, 0x02, 0x03, 0x70, 0x00}, 5);
    uint8_t test_packet[] = {0xfd, 0xff, 0x00, 0x02, 0x03, 0x70, 0x00,
                            ((crc & 0xff00) >> 8), (crc & 0xff), 0xfe};

    for (uint8_t ix = 0; ix != sizeof(test_packet); ix++) {
        transport_update(test_packet[ix]);
    }

    crc = calcCrc16CCITT((uint8_t []){test_addr, 0x02, 0x03, (0x80 | 0x70), 53, 11, 't', 'e', 's', 't', ' ', 'd', 'e', 'v', 'i', 'c', 'e',
            35, '0','.','0','.','1',' ','(','2','0','2','0','-','0','3','-','1','7',' ','a','a','b','b',
            'c','c','d','d','e','e','f','f','8','8','7','7',')',
            04, 't','e','s','t'}, 58);
    uint8_t expected[] = {0xfd, test_addr, 0x02, 0x03, (0x80 | 0x70), 53, 11, 't', 'e', 's', 't', ' ', 'd', 'e', 'v', 'i', 'c', 'e',
            35, '0','.','0','.','1',' ','(','2','0','2','0','-','0','3','-','1','7',' ','a','a','b','b',
            'c','c','d','d','e','e','f','f','8','8','7','7',')',
            04, 't','e','s','t', ((crc & 0xff00) >> 8), (crc & 0xff), 0xfe};
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected, trans_buffer, sizeof(expected));
}
