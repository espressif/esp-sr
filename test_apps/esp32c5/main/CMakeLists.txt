
set(srcs
    "app_main.cpp"
    "test_aec.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES unity esp-sr esp_timer
                    WHOLE_ARCHIVE)

