#include "rain_handler.h"
#include <esp_log.h>

static const char *TAG = "rain_handler";

static gpio_num_t rain_pin;
static bool initialized = false;

esp_err_t rain_handler_init(gpio_num_t pin) {
  rain_pin = pin;

  // Reseta o pino para garantir que não tenha configurações antigas
  esp_err_t err = gpio_reset_pin(rain_pin);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao resetar o GPIO %d: %s", pin, esp_err_to_name(err));
    return err;
  }

  // Configura o pino como entrada
  err = gpio_set_direction(rain_pin, GPIO_MODE_INPUT);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "Falha ao configurar a direção do GPIO %d: %s", pin,
             esp_err_to_name(err));
    return err;
  }

  initialized = true;
  ESP_LOGI(TAG, "Sensor de chuva configurado no GPIO %d", pin);
  return ESP_OK;
}

bool rain_handler_is_raining(void) {
  if (!initialized) {
    ESP_LOGE(TAG, "rain_handler_is_raining chamado antes de rain_handler_init");
    return false;
  }

  return (gpio_get_level(rain_pin) == 0);
}
