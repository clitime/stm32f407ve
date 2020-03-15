#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
#include <stdbool.h>

enum {
#ifndef TIMERS_AMOUNT
    TIMERS_AMOUNT = 3,
#endif
#ifndef TIMERS_INCREASE
    TIMERS_INCREASE = 1,
#endif
};


typedef uint16_t timer_t;
struct timer;


struct timer *timer_create(timer_t val);
void timer_remove(struct timer **tim);
void timer_process(void);

bool timer_start(struct timer *tim);
bool timer_reset(struct timer *tim, timer_t val);
bool timer_stop(struct timer *tim);
bool timer_is_timeout(struct timer *tim);
bool timer_is_stopped(struct timer *tim);


#endif
