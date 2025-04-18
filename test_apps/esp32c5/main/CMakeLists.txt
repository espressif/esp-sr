
set(srcs
    "app_main.cpp"
    "test_aec.cpp"
    "test_wakenet.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES unity esp-sr esp_timer
                    WHOLE_ARCHIVE)

