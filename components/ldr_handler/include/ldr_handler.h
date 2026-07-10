#ifndef LDR_HANDLER_H
#define LDR_HANDLER_H

#include <esp_err.h>
#include "driver/gpio.h"

#define LDR_HANDLER_DEFAULT_PIN GPIO_NUM_34

#define LDR_HANDLER_ADC_MAX 4095

esp_err_t ldr_handler_init(gpio_num_t pin);

esp_err_t ldr_handler_read_raw(int *raw);

#endif
