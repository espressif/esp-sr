
set(srcs
    "app_main.cpp"
    "test_wakenet.cpp"
    "test_multinet.cpp"
    "test_afe.cpp"
    "test_mfcc.cpp"
    "test_vadnet.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "samples"
                    REQUIRES unity esp-sr esp_timer
                    WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
