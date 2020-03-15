#include "unity.h"

#define RFIFO_QUANT 1
#include "ring_queue.h"


#define BUF_SIZE (8)
static uint8_t buf[BUF_SIZE];
static struct rfifo *test_fifo = NULL;


void setUp(void) {
    test_fifo = rfifo_create(buf, BUF_SIZE);
}


void tearDown(void) {
    rfifo_delete(test_fifo);
}


void suiteSetUp(void) {}
int suiteTearDown(int num_failures) {}


void test_createRQueue(void) {
    TEST_ASSERT(test_fifo != NULL);
}


void test_emptyQueue(void) {
    TEST_ASSERT(rfifo_is_empty(test_fifo));
}


void test_appendQueue(void) {
    const uint8_t exp_val = 5;
    rfifo_append(test_fifo, exp_val);
    TEST_ASSERT_EQUAL_UINT8(1, rfifo_get_length(test_fifo));
    uint8_t d = 0;
    TEST_ASSERT(rfifo_pop(test_fifo, &d));
    TEST_ASSERT_EQUAL_UINT8(exp_val, d);
}


void test_fullQueue(void) {
    for (uint8_t ix = 0; ix != BUF_SIZE; ix++) {
        TEST_ASSERT(rfifo_append(test_fifo, ix));
    }
    TEST_ASSERT_FALSE(rfifo_append(test_fifo, 100));
}


void test_infinityCycle(void) {
    uint8_t d = 0;
    for (uint8_t ix = 0; ix != sizeof(uint8_t) - 1; ix++) {
        TEST_ASSERT(rfifo_append(test_fifo, ix));
        TEST_ASSERT(rfifo_pop(test_fifo, &d));
        TEST_ASSERT_EQUAL_UINT8(ix, d);
    }
}


void test_eachSecondPop(void) {
    uint8_t d = 0;
    uint8_t iy = 0;
    uint8_t ix = 0;
    for (; ix != UINT8_MAX - 1 && !rfifo_is_full(test_fifo); ix++) {
        TEST_ASSERT(rfifo_append(test_fifo, ix));
        if ((ix % 2) == 0) {
            TEST_ASSERT(rfifo_pop(test_fifo, &d));
            TEST_ASSERT_EQUAL_UINT8(iy, d);
            iy++;
        }
    }

    uint8_t i = ix - BUF_SIZE;
    while (rfifo_pop(test_fifo, &d)) {
        TEST_ASSERT_EQUAL_UINT8(i, d);
        i++;
    }
}


void test_clear(void) {
    for (uint8_t ix = 0; ix != UINT8_MAX - 1 && !rfifo_is_full(test_fifo); ix++) {
        TEST_ASSERT(rfifo_append(test_fifo, ix));
    }

    rfifo_clear(test_fifo);
    TEST_ASSERT(rfifo_is_empty(test_fifo));
}
