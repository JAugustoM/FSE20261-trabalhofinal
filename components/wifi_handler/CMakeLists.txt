idf_component_register(SRCS "src/wifi_handler.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_wifi nvs_flash esp_event lwip)