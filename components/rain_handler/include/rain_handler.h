#ifndef RAIN_HANDLER_H
#define RAIN_HANDLER_H

#include "driver/gpio.h"
#include <esp_err.h>
#include <stdbool.h>

#define RAIN_HANDLER_DEFAULT_PIN GPIO_NUM_18

esp_err_t rain_handler_init(gpio_num_t pin);
bool rain_handler_is_raining(void);

#endif // RAIN_HANDLER_H
