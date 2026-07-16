idf_component_register(SRCS src/rain_handler.c
                       INCLUDE_DIRS "include"
                       REQUIRES driver esp_driver_gpio
)
