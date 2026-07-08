#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "bmp280_handler.h"

static const char* TAG = "estacao_esp";

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
}
