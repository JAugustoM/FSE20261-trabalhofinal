#ifndef MQTT_H
#define MQTT_H

#include "mqtt_client.h"

typedef struct {
  float temp;
  float pres;
  float hum;  
  int   lum;
} sensor_data;

extern esp_mqtt_client_handle_t connect_mqtt();
extern void publish_sensor_data(esp_mqtt_client_handle_t client, const sensor_data* data);

#endif
