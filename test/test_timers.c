#include "unity.h"

#define TIMERS_AMOUNT 1
#include "timers.h"


static struct timer *test_timer = 0;
#define TIMER_COUNT 5

void setUp(void) {
    test_timer = timer_create(TIMER_COUNT);
}

void tearDown(void) {
    timer_remove(&test_timer);
}

void suiteSetUp(void) {}
int suiteTearDown(int num_failures) {}


void test_timerStop(void) {
    TEST_ASSERT(test_timer);
    TEST_ASSERT(timer_is_stopped(test_timer));
    timer_start(test_timer);
    TEST_ASSERT(!timer_is_timeout(test_timer));
    TEST_ASSERT(!timer_is_stopped(test_timer));
    timer_stop(test_timer);
    TEST_ASSERT(timer_is_stopped(test_timer));
}


void test_timerProccesZeroStep(void) {
    TEST_ASSERT(test_timer);
    TEST_ASSERT(timer_is_stopped(test_timer));
    const uint8_t reset_val = 0;
    timer_reset(test_timer, reset_val);
    TEST_ASSERT(!timer_is_stopped(test_timer));

    for (uint8_t ix = 0; ix < reset_val + 1; ix++) {
        TEST_ASSERT(!timer_is_timeout(test_timer));
        timer_process();
    }
    TEST_ASSERT(timer_is_timeout(test_timer));
}


void test_timerProccesOneStep(void) {
    TEST_ASSERT(test_timer);
    TEST_ASSERT(timer_is_stopped(test_timer));
    const uint8_t reset_val = 1;
    timer_reset(test_timer, reset_val);
    TEST_ASSERT(!timer_is_stopped(test_timer));

    for (uint8_t ix = 0; ix < reset_val; ix++) {
        TEST_ASSERT(!timer_is_timeout(test_timer));
        timer_process();
    }
    TEST_ASSERT(timer_is_timeout(test_timer));
}


void test_timerProcces(void) {
    TEST_ASSERT(test_timer);
    TEST_ASSERT(timer_is_stopped(test_timer));
    timer_start(test_timer);
    TEST_ASSERT(!timer_is_stopped(test_timer));

    for (uint8_t ix = 0; ix < TIMER_COUNT; ix++) {
        TEST_ASSERT(!timer_is_timeout(test_timer));
        timer_process();
    }
    timer_process();
    TEST_ASSERT(timer_is_timeout(test_timer));
}
