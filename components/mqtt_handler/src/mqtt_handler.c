#include "cJSON.h"
#include "esp_err.h"
#include "esp_event_base.h"
#include "esp_log.h"
#include "freertos/idf_additions.h"
#include "mdns.h"
#include "portmacro.h"

#include "credentials.h"
#include "mqtt_handler.h"

static const char *TAG = "MQTT_HANDLER";

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
  esp_mqtt_event_handle_t event = event_data;
  switch ((esp_mqtt_event_id_t)event_id) {
    case MQTT_EVENT_CONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
      break;
    case MQTT_EVENT_DISCONNECTED:
      ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
      break;
    case MQTT_EVENT_PUBLISHED:
      ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
      break;
    case MQTT_EVENT_ERROR:
      ESP_LOGE(TAG, "MQTT_EVENT_ERROR");
      break;
    default:
      break;
  }
}

void initialize_mdns() {
    esp_err_t err = mdns_init();
    if (err) {
        ESP_LOGE("MDNS", "Failed to initialize mDNS: %s", esp_err_to_name(err));
        return;
    }
    
    mdns_hostname_set("estacao-esp32");
    mdns_instance_name_set("Estacao ESP32");
    
    ESP_LOGI("MDNS", "mDNS initialized successfully");
}

esp_mqtt_client_handle_t connect_mqtt() {
  esp_mqtt_client_config_t mqtt_cfg = {
    .broker.address.uri = BROKER_URI,
    .credentials = {
      .username = MQTT_USER,
      .authentication.password = MQTT_PASS,
    },
  }; 

  esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

  if (client != NULL) {
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
    return client;
  }

  ESP_LOGE(TAG, "Falha ao inicializar cliente MQTT");
  return NULL;
}

void publish_sensor_data(esp_mqtt_client_handle_t client, const sensor_data* data) {
  if (client == NULL || data == NULL) {
    ESP_LOGE(TAG, "Cliente MQTT ou dados invalidos");
    return;
  }

  cJSON *root = cJSON_CreateObject();
  if (root == NULL) {
    ESP_LOGE(TAG, "Falha ao criar objeto JSON");
    return;
  }

  cJSON_AddNumberToObject(root, "temp", data->temp);
  cJSON_AddNumberToObject(root, "pres", data->pres);
  cJSON_AddNumberToObject(root, "hum", data->hum);
  cJSON_AddNumberToObject(root, "lum", data->lum);

  char *json_string = cJSON_PrintUnformatted(root);
  if (json_string != NULL) {
    int msg_id = esp_mqtt_client_publish(client, "esp/data", json_string, 0, 1, 1);
    ESP_LOGI(TAG, "Mensagem enviada, msg_id=%d", msg_id);
    
    free(json_string);
  } else {
    ESP_LOGE(TAG, "Falha ao printar objeto JSON");
  }

  cJSON_Delete(root);
}
