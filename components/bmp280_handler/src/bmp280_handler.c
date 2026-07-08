#include "bmp280_handler.h"

#include <string.h>
#include <bmp280.h>
#include <i2cdev.h>
#include <esp_log.h>

static const char *TAG = "bmp280_handler";

static bmp280_t dev;
static bool is_bme280 = false;
static bool initialized = false;

esp_err_t bmp280_handler_init(gpio_num_t sda_gpio, gpio_num_t scl_gpio, uint8_t addr)
{
    esp_err_t err;

    err = i2cdev_init();
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao inicializar i2cdev: %s", esp_err_to_name(err));
        return err;
    }

    memset(&dev, 0, sizeof(dev));

    err = bmp280_init_desc(&dev, addr, I2C_NUM_0, sda_gpio, scl_gpio);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha em bmp280_init_desc: %s", esp_err_to_name(err));
        return err;
    }

    bmp280_params_t params;
    bmp280_init_default_params(&params);

    err = bmp280_init(&dev, &params);
    if (err != ESP_OK) {
        ESP_LOGE(TAG,
                 "Sensor nao encontrado no endereco 0x%02X (SDA=%d SCL=%d): %s. "
                 "Confira a fiacao e o endereco (tente 0x77).",
                 addr, sda_gpio, scl_gpio, esp_err_to_name(err));
        return err;
    }

    is_bme280 = (dev.id == BME280_CHIP_ID);
    ESP_LOGI(TAG, "Sensor detectado: %s (chip id 0x%02X) no endereco 0x%02X",
             is_bme280 ? "BME280 (com umidade)" : "BMP280 (temperatura/pressao)",
             dev.id, addr);

    initialized = true;
    return ESP_OK;
}

esp_err_t bmp280_handler_read(float *temperature, float *pressure, float *humidity)
{
    if (!initialized) {
        ESP_LOGE(TAG, "bmp280_handler_read chamado antes de bmp280_handler_init");
        return ESP_ERR_INVALID_STATE;
    }

    float t = 0.0f, p = 0.0f, h = 0.0f;
    esp_err_t err = bmp280_read_float(&dev, &t, &p, &h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha na leitura do sensor: %s", esp_err_to_name(err));
        return err;
    }

    if (temperature) *temperature = t;
    if (pressure)    *pressure = p;
    if (humidity)    *humidity = is_bme280 ? h : 0.0f;

    return ESP_OK;
}

bool bmp280_handler_has_humidity(void)
{
    return is_bme280;
}
