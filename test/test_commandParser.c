#include "unity.h"
#include "command_parser.h"
#include "packeti.h"
#include "mock_version.h"


void setUp(void) {}
void tearDown(void) {}
void suiteSetUp(void) {}
int suiteTearDown(int num_failures) {}


void test_getInfo(void) {
    struct packet inter_buf = {.addr = 0x01, .id=0x02, .sqn=0x03, .cmd=0x70, .len=0x00, .data = {0x00}};
    const char dev_name[] = "test device";
    get_device_name_ExpectAndReturn(dev_name);
    const char dev_ver[] = "0.0.1 (2020-03-17 aabbccddeeff8877)";
    get_device_version_ExpectAndReturn(dev_ver);
    const char dev_type[] = "test";
    get_device_type_ExpectAndReturn(dev_type);
    struct packet *actual = cmd_parser(&inter_buf);

    struct packet expected = {.addr = 0x00, .id=0x00, .sqn=0x00, .cmd=(0x70|0x80), .len=53, .data = {
            11, 't', 'e', 's', 't', ' ', 'd', 'e', 'v', 'i', 'c', 'e',
            35, '0','.','0','.','1',' ','(','2','0','2','0','-','0','3','-','1','7',' ','a','a','b','b',
            'c','c','d','d','e','e','f','f','8','8','7','7',')',
            04, 't','e','s','t'
        }
    };
    TEST_ASSERT_EQUAL_MEMORY(&expected, actual, sizeof(struct packet));
}
