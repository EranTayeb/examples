
#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_intr_alloc.h"
#include "esp_timer.h"

static const char *TAG = "example";

#define LED_GPIO 2
#define BUTTON_GPIO 4
#define ESP_INTR_FLAG_DEFAULT 0
#define DEBOUNCE_DELAY_MS 200 

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK
} led_state_t;

volatile led_state_t led_state = LED_OFF;

void  isr_handler(void* arg);

void  isr_handler(void* arg) {
    static uint32_t last_isr_time = 0;
    uint32_t current_time = esp_timer_get_time() / 1000;  // Convert microseconds to milliseconds

    if ((current_time - last_isr_time) > DEBOUNCE_DELAY_MS) {
        // Cycle through the LED states
        if (led_state == LED_OFF) {
            led_state = LED_ON;
        } else if (led_state == LED_ON) {
            led_state = LED_BLINK;
        } else {
            led_state = LED_OFF;
        }
        ESP_EARLY_LOGI(TAG, "Button pressed, LED state: %d", led_state);
        last_isr_time = current_time;
    }
}

void blink_led(void) {
    int blink_delay = 500; // milliseconds

    while (1) {
        switch (led_state) {
            case LED_OFF:
                gpio_set_level(LED_GPIO, 0);
                break;
            case LED_ON:
                gpio_set_level(LED_GPIO, 1);
                break;
            case LED_BLINK:
                gpio_set_level(LED_GPIO, 1);
                esp_rom_delay_us(blink_delay * 1000);
                gpio_set_level(LED_GPIO, 0);
                esp_rom_delay_us(blink_delay * 1000);
                break;
        }
    }
}

void app_main(void) {
    ESP_LOGI(TAG, "Configuring GPIO %d as output and GPIO %d as input", LED_GPIO, BUTTON_GPIO);

    // Configure the LED pin as output
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure the button pin as input with pull-up resistor and interrupt
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_intr_type(BUTTON_GPIO, GPIO_INTR_NEGEDGE);

    // Install GPIO ISR service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    gpio_isr_handler_add(BUTTON_GPIO, isr_handler, (void*) BUTTON_GPIO);

    // Start the LED blinking logic
    blink_led();
}
