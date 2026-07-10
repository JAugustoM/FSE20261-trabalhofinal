#ifndef DHT_HANDLER_H
#define DHT_HANDLER_H

#include <esp_err.h>
#include "driver/gpio.h"

#define DHT_HANDLER_DEFAULT_PIN GPIO_NUM_4

esp_err_t dht_handler_init(gpio_num_t pin);

esp_err_t dht_handler_read(float *temperature, float *humidity);

#endif
