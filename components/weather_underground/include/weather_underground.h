#ifndef WEATHER_UNDERGROUND_H
#define WEATHER_UNDERGROUND_H

#include <stdint.h>
#include "esp_err.h"

typedef struct {
    float temperature;
} wu_data_t;

// initialize with weather underground credentials
void wu_init(const char* station_id, const char* station_key);

// convert to imperial and send request to weather underground
esp_err_t wu_send_data(const wu_data_t *data);

#endif