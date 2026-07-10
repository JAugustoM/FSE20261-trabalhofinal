#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h"
#include "sensor_data.h"

extern esp_mqtt_client_handle_t connect_mqtt();
extern void initialize_mdns();
extern void publish_sensor_data(esp_mqtt_client_handle_t client,
                                const sensor_data_t *data);

#endif
