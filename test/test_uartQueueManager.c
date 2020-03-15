#include "unity.h"
#include "uart_queue_manager.h"
#include "ring_queue.h"
#include <string.h>


enum {
    BUFFER_SIZE = QUEUE_SIZE,
};
static uint8_t actual_buffer[BUFFER_SIZE] = {0};
static uint8_t actual_ix = 0;

static void t_observer(uint8_t d) {
    if (actual_ix < BUFFER_SIZE) {
        actual_buffer[actual_ix++] = d;
    }
}

void setUp(void) {
    memset(actual_buffer, 0xff, BUFFER_SIZE);
    actual_ix = 0;
}

void tearDown(void) {}

void suiteSetUp(void) {
    queue_init();
    registry_observer(t_observer);
}

int suiteTearDown(int num_failures) {}


void test_notifiObserver(void) {
    uint8_t expected_buf[BUFFER_SIZE] = {0};
    for (uint8_t ix = 0; ix != BUFFER_SIZE; ix++) {
        uint8_t datum = ix + 100;
        expected_buf[ix] = datum;
        queue_append(datum);
    }
    notify_observers();
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buf, actual_buffer, BUFFER_SIZE);
}


void test_notifiObserverForEach(void) {
    uint8_t expected_buf[BUFFER_SIZE] = {0};
    for (uint8_t ix = 0; ix != BUFFER_SIZE; ix++) {
        uint8_t datum = ix;
        expected_buf[ix] = datum;
        queue_append(datum);
        notify_observers();
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buf, actual_buffer, BUFFER_SIZE);
}


void test_notifiObserverForEachFive(void) {
    const uint8_t each_elem = 5;
    const uint8_t buf_size = BUFFER_SIZE - (BUFFER_SIZE % each_elem);
    uint8_t expected_buf[BUFFER_SIZE] = {0};
    for (uint8_t ix = 0; ix <= buf_size; ix++) {
        uint8_t datum = ix;
        expected_buf[ix] = datum;
        queue_append(datum);
        if (ix % each_elem == 0)
            notify_observers();
    }
    TEST_ASSERT_EQUAL_UINT8_ARRAY(expected_buf, actual_buffer, buf_size);
}
