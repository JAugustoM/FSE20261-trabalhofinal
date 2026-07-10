idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES wifi_handler weather_underground bmp280_handler dht_handler ldr_handler mqtt_handler sensor_data)
