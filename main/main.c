#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_handler.h"
#include "weather_underground.h"
#include "bmp280_handler.h"
#include "dht_handler.h"
#include "ldr_handler.h"

#define UPDATE_INTERVAL CONFIG_ESP_WU_UPDATE_INTERVAL_MS

static const char* TAG = "estacao_esp";

SemaphoreHandle_t wifi_connection_semaphore;

void upload_weather_data_task(void *params) {
  if(xSemaphoreTake(wifi_connection_semaphore, portMAX_DELAY)) {

    ESP_LOGI(TAG, "WiFi conectado");
    wu_init(CONFIG_ESP_WU_STATION_ID, CONFIG_ESP_WU_STATION_KEY);

    // mock some data for testing
    wu_data_t mock_data = {
      .temperature = 25.0f
    };

    while(true) {
      ESP_LOGI(TAG, "Enviando dados para o Weather Underground...");
      wu_send_data(&mock_data);

      vTaskDelay(pdMS_TO_TICKS(UPDATE_INTERVAL));
    }
  }
}

void app_main(void) {
  // Inicializa os sensores (aborta se algum init obrigatorio falhar)
  ESP_ERROR_CHECK(bmp280_handler_init(BMP280_HANDLER_DEFAULT_SDA,
                                      BMP280_HANDLER_DEFAULT_SCL,
                                      BMP280_HANDLER_DEFAULT_ADDR));
  ESP_ERROR_CHECK(dht_handler_init(DHT_HANDLER_DEFAULT_PIN));
  ESP_ERROR_CHECK(ldr_handler_init(LDR_HANDLER_DEFAULT_PIN));

  // O DHT11 precisa de ~1s apos ligar para estabilizar; sem essa espera a
  // primeira leitura costuma falhar (timeout na "phase B").
  vTaskDelay(pdMS_TO_TICKS(1000));

  while (1) {
    float bmp_temp = 0.0f, bmp_pressao = 0.0f;
    float dht_temp = 0.0f, dht_umidade = 0.0f;

    if (bmp280_handler_read(&bmp_temp, &bmp_pressao, NULL) == ESP_OK) {
      ESP_LOGI(TAG, "[BMP280] Temp: %.2f C | Pressao: %.2f Pa", bmp_temp, bmp_pressao);
    }

    if (dht_handler_read(&dht_temp, &dht_umidade) == ESP_OK) {
      ESP_LOGI(TAG, "[DHT11]  Temp: %.1f C | Umidade: %.1f %%", dht_temp, dht_umidade);
    }

    int luz_raw = 0;
    if (ldr_handler_read_raw(&luz_raw) == ESP_OK) {
      int luz_pct = luz_raw * 100 / LDR_HANDLER_ADC_MAX;
      ESP_LOGI(TAG, "[GBK P7] Luminosidade: raw=%d (%d%%)", luz_raw, luz_pct);
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }

  // initialize wifi and create a task to upload weather data
  // esp_err_t ret = nvs_flash_init();
  // if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
  //   ESP_ERROR_CHECK(nvs_flash_erase());
  //   ret = nvs_flash_init();
  // }
  // ESP_ERROR_CHECK(ret);

  // wifi_connection_semaphore = xSemaphoreCreateBinary();
  // wifi_start();
  // xTaskCreate(&upload_weather_data_task, "upload_weather_data_task", 4096, NULL, 5, NULL);
}