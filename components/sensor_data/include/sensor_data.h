#ifndef SENSOR_DATA_H
#define SENSOR_DATA_H

typedef struct {
  float temp; // from BMP280
  float pres; // from BMP280
  float hum;  // from DHT11
  int lum;    // value between 0 and 4095 coming from the LDR
} sensor_data_t;

#endif // !SENSOR_DATA_H
