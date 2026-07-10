#include "dht_handler.h"

#include <dht.h>
#include <esp_log.h>

static const char *TAG = "dht_handler";

static const dht_sensor_type_t SENSOR_TYPE = DHT_TYPE_DHT11;

static gpio_num_t dht_pin;
static bool initialized = false;

esp_err_t dht_handler_init(gpio_num_t pin)
{
    dht_pin = pin;
    initialized = true;
    ESP_LOGI(TAG, "DHT11 configurado no GPIO %d", pin);
    return ESP_OK;
}

esp_err_t dht_handler_read(float *temperature, float *humidity)
{
    if (!initialized) {
        ESP_LOGE(TAG, "dht_handler_read chamado antes de dht_handler_init");
        return ESP_ERR_INVALID_STATE;
    }

    // Retorna os valores nesta ordem: umidade, temperatura
    float h = 0.0f, t = 0.0f;
    esp_err_t err = dht_read_float_data(SENSOR_TYPE, dht_pin, &h, &t);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha na leitura do DHT11 (GPIO %d): %s",
                 dht_pin, esp_err_to_name(err));
        return err;
    }

    if (temperature) *temperature = t;
    if (humidity)    *humidity = h;

    return ESP_OK;
}
