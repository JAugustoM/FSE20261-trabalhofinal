idf_component_register(SRCS src/bmp280_handler.c
                       INCLUDE_DIRS "include"
                       REQUIRES bmp280 i2cdev)
