
set(srcs
    "main.c"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "." "samples"
                    REQUIRES unity esp-sr
                    WHOLE_ARCHIVE)

target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
