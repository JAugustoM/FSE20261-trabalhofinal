#include "bmp280_handler.h"
#include "dht_handler.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h" // Inclusão obrigatória para Mutex
#include "freertos/task.h"
#include "ldr_handler.h"
#include "mqtt_handler.h"
#include "nvs_flash.h"
#include "sensor_data.h"
#include "weather_underground.h"
#include "wifi_handler.h"

#define UPDATE_INTERVAL CONFIG_ESP_WU_UPDATE_INTERVAL_MS

static const char *TAG = "ESTACAO_ESP";

SemaphoreHandle_t wifi_connection_semaphore;
SemaphoreHandle_t data_mutex;

sensor_data_t data = {
    .temp = 0,
    .hum = 0,
    .pres = 0,
    .lum = 0,
};

void upload_weather_data_task(void *params) {
  if (xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY)) {
    xSemaphoreGive(wifi_connection_semaphore);

    ESP_LOGI(TAG, "WiFi conectado");
    wu_init(CONFIG_ESP_WU_STATION_ID, CONFIG_ESP_WU_STATION_KEY);

    while (true) {
      ESP_LOGI(TAG, "Enviando dados para o Weather Underground...");

      sensor_data_t local_data;
      if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
        local_data = data;
        xSemaphoreGive(data_mutex);
      }

      wu_send_data(&local_data);
      vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
  }
}

void upload_mqtt_data_task(void *params) {
  esp_mqtt_client_handle_t client = (esp_mqtt_client_handle_t)params;

  if (xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY)) {
    xSemaphoreGive(wifi_connection_semaphore);

    while (true) {
      ESP_LOGI(TAG, "Enviando dados MQTT...");

      sensor_data_t local_data;
      if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
        local_data = data;
        xSemaphoreGive(data_mutex);
      }

      publish_sensor_data(client, &local_data);

      vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
  }
}

void app_main(void) {
  // Inicialização do Mutex
  data_mutex = xSemaphoreCreateMutex();

  ESP_ERROR_CHECK(bmp280_handler_init(BMP280_HANDLER_DEFAULT_SDA,
                                      BMP280_HANDLER_DEFAULT_SCL,
                                      BMP280_HANDLER_DEFAULT_ADDR));
  ESP_ERROR_CHECK(dht_handler_init(DHT_HANDLER_DEFAULT_PIN));
  ESP_ERROR_CHECK(ldr_handler_init(LDR_HANDLER_DEFAULT_PIN));

  vTaskDelay(pdMS_TO_TICKS(1000));

  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  wifi_connection_semaphore = xSemaphoreCreateBinary();
  wifi_start();

  esp_mqtt_client_handle_t mqtt_client = connect_mqtt();

  xTaskCreate(&upload_weather_data_task, "upload_weather_data_task", 4096, NULL,
              5, NULL);

  xTaskCreate(&upload_mqtt_data_task, "upload_mqtt_data_task", 4096,
              mqtt_client, 5, NULL);

  while (1) {
    float bmp_temp = 0.0f, bmp_pressao = 0.0f;
    float dht_temp = 0.0f, dht_umidade = 0.0f;
    int luz_raw = 0;

    if (bmp280_handler_read(&bmp_temp, &bmp_pressao, NULL) == ESP_OK) {
      ESP_LOGI(TAG, "[BMP280] Temp: %.2f C | Pressao: %.2f Pa", bmp_temp,
               bmp_pressao);
    }

    if (dht_handler_read(&dht_temp, &dht_umidade) == ESP_OK) {
      ESP_LOGI(TAG, "[DHT11]  Temp: %.1f C | Umidade: %.1f %%", dht_temp,
               dht_umidade);
    }

    if (ldr_handler_read_raw(&luz_raw) == ESP_OK) {
      int luz_pct = luz_raw * 100 / LDR_HANDLER_ADC_MAX;
      ESP_LOGI(TAG, "[GBK P7] Luminosidade: raw=%d (%d%%)", luz_raw, luz_pct);
    }

    if (xSemaphoreTake(data_mutex, portMAX_DELAY)) {
      data.temp = bmp_temp;
      data.pres = bmp_pressao;
      data.hum = dht_umidade;
      data.lum = luz_raw;
      xSemaphoreGive(data_mutex);
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}
