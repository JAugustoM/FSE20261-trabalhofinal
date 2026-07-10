idf_component_register(SRCS src/mqtt_handler.c
                       INCLUDE_DIRS "include"
                       REQUIRES mqtt sensor_data
                       PRIV_REQUIRES cjson mdns)

