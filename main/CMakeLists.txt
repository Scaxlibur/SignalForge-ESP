idf_component_register(SRCS "wave_gen.cpp" "AD5626.c" "settingSave.cpp" "AD9834.cpp" "main.cpp"
                    INCLUDE_DIRS "."
                    REQUIRES nvs_flash)