/* Button Controlled LED with Three States Example in Bare Metal
   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "example";

#define LED_GPIO 2
#define BUTTON_GPIO 4

typedef enum {
    LED_OFF,
    LED_ON,
    LED_BLINK
} led_state_t;

void app_main(void)
{
    ESP_LOGI(TAG, "Configuring GPIO %d as output and GPIO %d as input", LED_GPIO, BUTTON_GPIO);

    // Configure the LED pin as output
    gpio_reset_pin(LED_GPIO);
    gpio_set_direction(LED_GPIO, GPIO_MODE_OUTPUT);

    // Configure the button pin as input with pull-up resistor
    gpio_reset_pin(BUTTON_GPIO);
    gpio_set_direction(BUTTON_GPIO, GPIO_MODE_INPUT);
    gpio_set_pull_mode(BUTTON_GPIO, GPIO_PULLUP_ONLY);

    // Initialize LED state and button state
    led_state_t led_state = LED_OFF;
    uint8_t button_state = 0;
    uint8_t last_button_state = 1;
    int blink_delay = 500; // milliseconds

    while (1) {
        // Read the button state
        button_state = gpio_get_level(BUTTON_GPIO);

        // Check for button press (transition from high to low)
        if (button_state == 0 && last_button_state == 1) {
            // Cycle through the LED states
            if (led_state == LED_OFF) {
                led_state = LED_ON;
            } else if (led_state == LED_ON) {
                led_state = LED_BLINK;
            } else {
                led_state = LED_OFF;
            }

            ESP_LOGI(TAG, "Button pressed, LED state: %d", led_state);

            // Debounce delay
            esp_rom_delay_us(200000);  // 200ms delay to debounce button
        }

        // Update last button state
        last_button_state = button_state;

        // Control the LED based on the state
        switch (led_state) {
            case LED_OFF:
                gpio_set_level(LED_GPIO, 0);
                break;
            case LED_ON:
                gpio_set_level(LED_GPIO, 1);
                break;
            case LED_BLINK:
                gpio_set_level(LED_GPIO, 1);
                for (int i = 0; i < blink_delay; i += 10) {
                    esp_rom_delay_us(10000);  // 10ms delay
                    button_state = gpio_get_level(BUTTON_GPIO);
                    if (button_state == 0 && last_button_state == 1) {
                        // Cycle through the LED states
                        if (led_state == LED_OFF) {
                            led_state = LED_ON;
                        } else if (led_state == LED_ON) {
                            led_state = LED_BLINK;
                        } else {
                            led_state = LED_OFF;
                        }
                        ESP_LOGI(TAG, "Button pressed, LED state: %d", led_state);
                        esp_rom_delay_us(200000);  // 200ms delay to debounce button
                        break;
                    }
                    last_button_state = button_state;
                }
                gpio_set_level(LED_GPIO, 0);
                for (int i = 0; i < blink_delay; i += 10) {
                    esp_rom_delay_us(10000);  // 10ms delay
                    button_state = gpio_get_level(BUTTON_GPIO);
                    if (button_state == 0 && last_button_state == 1) {
                        // Cycle through the LED states
                        if (led_state == LED_OFF) {
                            led_state = LED_ON;
                        } else if (led_state == LED_ON) {
                            led_state = LED_BLINK;
                        } else {
                            led_state = LED_OFF;
                        }
                        ESP_LOGI(TAG, "Button pressed, LED state: %d", led_state);
                        esp_rom_delay_us(200000);  // 200ms delay to debounce button
                        break;
                    }
                    last_button_state = button_state;
                }
                break;
        }

        // Short delay to avoid excessive CPU usage
        esp_rom_delay_us(10000);  // 10ms delay
    }
}
