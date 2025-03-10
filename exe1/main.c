#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include "hardware/timer.h"
#include <stdio.h>
const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;


bool led_callback(struct repeating_timer *t){

    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R)); 
    return true;

}

void start_timer(struct repeating_timer *t) {
    

    add_repeating_timer_ms(500, led_callback, NULL, t);

}

void stop_timer(struct repeating_timer *t) {

    cancel_repeating_timer(t);
    gpio_put(LED_PIN_R, 0);  

}


void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = !flag_f_r;
    } else if (events == 0x8) { // rise edge
    }
}

int main() {

    bool timer_active = false;
    struct repeating_timer timer;

    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    
    
    while (true) {

        if (flag_f_r && timer_active == false) {
            start_timer(&timer); 
            timer_active = true;
        }
        else if (!flag_f_r && timer_active == true){
            stop_timer(&timer);
            timer_active = false;
        }
    }
}
