#include "timers.h"
#include <stddef.h>


struct timer {
    timer_t value;
    timer_t counter;
    uint8_t state;
};


enum timer_state {
    TIMER_STOPPED = 0,
    TIMER_RUNNING,
    TIMER_TIMEOUT
};


#define assert_timers(t)    \
    do {                    \
        if ((t) == NULL) {  \
            return false;   \
        }                   \
    }while(0)


static struct timer timers[TIMERS_AMOUNT] = {0};
static bool is_busy[TIMERS_AMOUNT] = {0};


struct timer *timer_create(timer_t val) {
    uint32_t ix = 0;
    for (; ix < TIMERS_AMOUNT && is_busy[ix]; ix++) {
        continue;
    }

    if (ix >= TIMERS_AMOUNT) {
        return NULL;
    }

    timers[ix].value = val;
    timers[ix].counter = val;
    timers[ix].state = TIMER_STOPPED;

    is_busy[ix] = true;
    return &timers[ix];
}


void timer_remove(struct timer **tim) {
    for (uint8_t i = 0; i != TIMERS_AMOUNT; i++) {
        if (&timers[i] == *tim) {
            is_busy[i] = false;
            *tim = 0;
            timers[i].value = 0;
            timers[i].counter = 0;
            timers[i].state = TIMER_STOPPED;
        }
    }
}


void timer_process(void) {
    for (uint32_t ix = 0; ix < TIMERS_AMOUNT; ix++) {
        if (is_busy[ix] == false) {
            continue;
        }

        if (timers[ix].state == TIMER_STOPPED) {
            continue;
        }

        if (timers[ix].counter < timers[ix].value) {
            timers[ix].counter += TIMERS_INCREASE;
            if (timers[ix].counter != timers[ix].value) {
                continue;
            }
        }

        timers[ix].state = TIMER_TIMEOUT;
    }
}


bool timer_start(struct timer *tim) {
    assert_timers(tim);
    tim->state = TIMER_RUNNING;
    tim->counter = 0;
    return true;
}


bool timer_reset(struct timer *tim, timer_t val) {
    assert_timers(tim);

    tim->state = TIMER_RUNNING;
    tim->counter = 0;
    tim->value = val;
    return true;
}


bool timer_stop(struct timer *tim) {
    assert_timers(tim);

    tim->state = TIMER_STOPPED;
    tim->counter = tim->value;
    return true;
}


bool timer_is_timeout(struct timer *tim) {
    assert_timers(tim);

    if (tim->state != TIMER_TIMEOUT) {
        return false;
    }

    return true;
}


bool timer_is_stopped(struct timer *tim) {
    assert_timers(tim);

    if (tim->state != TIMER_STOPPED) {
        return false;
    }

    return true;
}
