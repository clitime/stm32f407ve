#include "reciever.h"
#include "stm32f4xx.h"
#include <stdbool.h>
#include <tim6.h>
#include "inout.h"
#include "timers.h"
#include "uart.h"
#include "uart_queue_manager.h"


int main (void) {
    tim6_configure();

    config_output();
    queue_init();
    transport_init(put_into_transmit_buffer);
    registry_observer(transport_update);

    uart_init(UART_9600);

    set_output_state(OUT_LED_3, io_set);
    set_output_state(OUT_LED_4, io_reset);
    struct timer *tim_led3 = timer_create(200);
    struct timer *tim_led4 = timer_create(150);
    struct timer *tim_msg = timer_create(5000);
    timer_start(tim_led3);
    timer_start(tim_led4);

    put_into_transmit_buffer((uint8_t[]){"start"}, sizeof("start"));
    timer_start(tim_msg);

    while (true) {
        notify_observers();
        if (timer_is_timeout(tim_led3)) {
            toggle_output_state(OUT_LED_3);
            timer_start(tim_led3);
        }
        if (timer_is_timeout(tim_led4)) {
            toggle_output_state(OUT_LED_4);
            timer_start(tim_led4);
        }
    }
}
