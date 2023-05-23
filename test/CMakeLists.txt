set(srcs
    test_chinese_tts.c
)

if(IDF_TARGET STREQUAL "esp32")
    list(APPEND srcs test_wakenet.c
                    test_afe.c 
                    test_multinet.c)

elseif(IDF_TARGET STREQUAL "esp32s3")
    list(APPEND srcs test_wakenet.c
                    test_afe.c 
                    test_multinet.c)

endif()

idf_component_register(SRCS ${srcs}
                        INCLUDE_DIRS "."
                        REQUIRES cmock esp-sr)
