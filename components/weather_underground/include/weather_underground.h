#ifndef WEATHER_UNDERGROUND_H
#define WEATHER_UNDERGROUND_H

#include "esp_err.h"
#include "sensor_data.h"
#include <stdint.h>

// initialize with weather underground credentials
void wu_init(const char *station_id, const char *station_key);

// convert to imperial and send request to weather underground
esp_err_t wu_send_data(const sensor_data_t *data);

#endif
