if(IDF_TARGET STREQUAL "esp32s3")

set(srcs
    "test_app_main.c"
    "test_wakenet.c"
    "test_multinet.c"
    "test_afe.c"
    "test_chinese_tts.c"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES unity esp-sr
                    WHOLE_ARCHIVE)


endif()