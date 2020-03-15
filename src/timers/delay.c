#include "delay.h"


static volatile uint32_t timeout_msec = 0;


void dec_delay_msec(void) {
    if (timeout_msec) {
        --timeout_msec;
    }
}


void delay_msec(uint32_t msec) {
    timeout_msec = msec;
    while (timeout_msec) {
        continue;
    }
}
