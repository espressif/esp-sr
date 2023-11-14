
set(srcs
    "test_app_main.c"
    "test_wakenet.c"
    "test_multinet.c"
    "test_afe.c"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "samples"
                    REQUIRES unity esp-sr
                    WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
