#include <stdio.h>
#include "nvs_flash.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "wifi_handler.h"
#include "weather_underground.h"
#include "bmp280_handler.h"

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
  // Inicializa o sensor (aborta se nao for encontrado, para deixar o erro claro no log)
  ESP_ERROR_CHECK(bmp280_handler_init(BMP280_HANDLER_DEFAULT_SDA,
                                      BMP280_HANDLER_DEFAULT_SCL,
                                      BMP280_HANDLER_DEFAULT_ADDR));

  while (1) {
    float temperatura = 0.0f, pressao = 0.0f;

    if (bmp280_handler_read(&temperatura, &pressao, NULL) == ESP_OK) {
      ESP_LOGI(TAG, "[BMP280] Temp: %.2f C | Pressao: %.2f Pa", temperatura, pressao);
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