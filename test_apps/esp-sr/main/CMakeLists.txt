
set(srcs
    "app_main.cpp"
    "test_wakenet.cpp"
    "test_multinet.cpp"
    "test_afe.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "samples"
                    REQUIRES unity esp-sr
                    WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
