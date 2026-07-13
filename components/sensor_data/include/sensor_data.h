#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

#include <stdbool.h>

typedef struct {
  float temp;
  float pres;
  float hum;
  int lum;
  bool is_raining;
} sensor_data_t;

#endif // !SENSOR_DATA_H
