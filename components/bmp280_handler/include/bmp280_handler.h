#ifndef BMP280_HANDLER_H
#define BMP280_HANDLER_H

#include <stdbool.h>
#include "driver/gpio.h"
#include <esp_err.h>

#define BMP280_HANDLER_DEFAULT_SDA  GPIO_NUM_21
#define BMP280_HANDLER_DEFAULT_SCL  GPIO_NUM_22
#define BMP280_HANDLER_DEFAULT_ADDR 0x76

esp_err_t bmp280_handler_init(gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint8_t addr);

esp_err_t bmp280_handler_read(float *temperature, float *pressure, float *humidity);

bool bmp280_handler_has_humidity(void);

#endif
