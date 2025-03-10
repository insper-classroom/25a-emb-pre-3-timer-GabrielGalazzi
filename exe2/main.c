#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;

// da pra fazer com um ledCallback se usar timer global?

bool led_callback_red(struct repeating_timer *t){

    gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
    return true;

}

bool led_callback_green(struct repeating_timer *t){

    gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
    return true;

}
void start_timer(struct repeating_timer *t, int delay_ms, int ledPin) {
    if (ledPin == LED_PIN_R){
        add_repeating_timer_ms(delay_ms, led_callback_red, NULL, t);
    }
    else if (ledPin == LED_PIN_G){
        add_repeating_timer_ms(delay_ms, led_callback_green, NULL, t);
    }
}

void stop_timer(struct repeating_timer *t, int ledPin) {

    cancel_repeating_timer(t);
    gpio_put(ledPin, 0);  

}


void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_r = !flag_r;
        else if (gpio == BTN_PIN_G)
            flag_g = !flag_g;
    }
}

int main() {

    bool timer_active_red = false;
    bool timer_active_green = false;
    struct repeating_timer timer_red;
    struct repeating_timer timer_green;


    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    while (true) {

        if (flag_r && timer_active_red == false) {
            start_timer(&timer_red, 500, LED_PIN_R); 
            timer_active_red = true;
        }
        else if (!flag_r && timer_active_red == true){
            stop_timer(&timer_red, LED_PIN_R);
            timer_active_red = false;
        }

        if (flag_g && timer_active_green == false) {
            start_timer(&timer_green, 250, LED_PIN_G); 
            timer_active_green = true;
        }
        else if (!flag_g && timer_active_green == true){
            stop_timer(&timer_green, LED_PIN_G);
            timer_active_green = false;
        }
    }
}
