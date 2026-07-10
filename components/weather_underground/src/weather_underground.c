#include "weather_underground.h"
#include "esp_http_client.h"
#include "esp_crt_bundle.h"
#include "esp_log.h"
#include <string.h>
#include <stdio.h>

static const char *TAG = "WU_CLIENT";
static char s_station_id[32];
static char s_station_key[32];

#define WU_BASE_URL "https://rtupdate.wunderground.com/weatherstation/updateweatherstation.php"

// struct for imperial data
typedef struct {
    float tempeture_f;
} wu_data_t;

// metric to imperial math
static wu_data_t convert_metric_to_imperial(const sensor_data_t *data) {
    wu_data_t imperial;
    imperial.tempeture_f = (data->temp * 9.0f / 5.0f) + 32.0f;
    return imperial;
}

void wu_init(const char* station_id, const char* station_key) {
    strncpy(s_station_id, station_id, sizeof(s_station_id) - 1);
    strncpy(s_station_key, station_key, sizeof(s_station_key) - 1);
    ESP_LOGI(TAG, "Iniciando estação com ID: %s", s_station_id);
}

esp_err_t wu_send_data(const sensor_data_t *data) {
    wu_data_t imperial = convert_metric_to_imperial(data);

    ESP_LOGI(TAG, "--------- Dados Meteorológicos ---------");
    ESP_LOGI(TAG, "Temperatura:     %.2f C   -> %.2f F", data->temp, imperial.tempeture_f);
    ESP_LOGI(TAG, "----------------------------------------");

    char url[512];
    snprintf(
        url,
        sizeof(url),
        "%s?ID=%s&PASSWORD=%s&dateutc=now&action=updateraw&tempf=%.1f&realtime=1&rtfreq=60",
        WU_BASE_URL,
        s_station_id,
        s_station_key,
        imperial.tempeture_f
    );

    esp_http_client_config_t config = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .crt_bundle_attach = esp_crt_bundle_attach,
    };

    ESP_LOGI(TAG, "URL: %s", url);
    
    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        int status_code = esp_http_client_get_status_code(client);
        if (status_code == 200) {
            ESP_LOGI(TAG, "Requisição HTTP bem-sucedida");
        } else {
            ESP_LOGW(TAG, "Status HTTP inesperado: %d", status_code);
        }
    } else {
        ESP_LOGE(TAG, "Falha na requisição HTTP: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}