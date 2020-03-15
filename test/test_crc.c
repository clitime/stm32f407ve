#include "unity.h"
#include "crc.h"


void setUp(void) {}
void tearDown(void) {}
void suiteSetUp(void) {}
int suiteTearDown(int num_failures) {}


void test_Crc16CCITT(void) {
    TEST_ASSERT_EQUAL_UINT16(0x29B1, calcCrc16CCITT((uint8_t []){"123456789"}, 9));
}
