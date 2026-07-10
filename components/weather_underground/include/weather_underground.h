#ifndef WEATHER_UNDERGROUND_H
#define WEATHER_UNDERGROUND_H

#include <stdint.h>
#include "esp_err.h"

typedef struct {
  float temp; // from BMP280
  float pres; // from BMP280
  float hum;  // from DHT11
  int lum;    // value between 0 and 4095 coming from the LDR
} sensor_data_t;

// initialize with weather underground credentials
void wu_init(const char* station_id, const char* station_key);

// convert to imperial and send request to weather underground
esp_err_t wu_send_data(const sensor_data_t *data);

#endif