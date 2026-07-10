#include "ldr_handler.h"

#include <esp_log.h>
#include "esp_adc/adc_oneshot.h"

static const char *TAG = "ldr_handler";

static adc_oneshot_unit_handle_t adc_handle = NULL;
static adc_channel_t adc_channel;
static bool initialized = false;

esp_err_t ldr_handler_init(gpio_num_t pin)
{
    esp_err_t err;
    adc_unit_t unit;

    err = adc_oneshot_io_to_channel(pin, &unit, &adc_channel);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "GPIO %d nao e um pino de ADC valido: %s", pin, esp_err_to_name(err));
        return err;
    }

    if (unit != ADC_UNIT_1) {
        ESP_LOGE(TAG, "GPIO %d esta no ADC2, que conflita com o WiFi. Use um pino do ADC1 (32-39).", pin);
        return ESP_ERR_INVALID_ARG;
    }

    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = unit,
    };
    err = adc_oneshot_new_unit(&unit_cfg, &adc_handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao criar unidade ADC: %s", esp_err_to_name(err));
        return err;
    }

    adc_oneshot_chan_cfg_t chan_cfg = {
        .atten = ADC_ATTEN_DB_12,          // faixa completa (~0 a 3.3V)
        .bitwidth = ADC_BITWIDTH_DEFAULT,  // 12 bits no ESP32 (0..4095)
    };
    err = adc_oneshot_config_channel(adc_handle, adc_channel, &chan_cfg);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Falha ao configurar canal ADC: %s", esp_err_to_name(err));
        return err;
    }

    initialized = true;
    ESP_LOGI(TAG, "Sensor de luz analogico (GBK P7) no GPIO %d (ADC1 canal %d)",
             pin, adc_channel);
    return ESP_OK;
}

esp_err_t ldr_handler_read_raw(int *raw)
{
    if (!initialized) {
        ESP_LOGE(TAG, "ldr_handler_read_raw chamado antes de ldr_handler_init");
        return ESP_ERR_INVALID_STATE;
    }

    return adc_oneshot_read(adc_handle, adc_channel, raw);
}
