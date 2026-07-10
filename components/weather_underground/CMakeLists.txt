idf_component_register(SRCS "src/weather_underground.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_http_client mbedtls)