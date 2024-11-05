
set(srcs
    "app_main.cpp"
    "test_chinese_tts.cpp"
)

idf_component_register(SRCS ${srcs}
                    INCLUDE_DIRS "."
                    REQUIRES unity esp-sr
                    WHOLE_ARCHIVE)



set(voice_data_image ${PROJECT_DIR}/../../esp-tts/esp_tts_chinese/esp_tts_voice_data_xiaoxin_small.dat)
add_custom_target(voice_data ALL DEPENDS ${voice_data_image})
add_dependencies(flash voice_data)

partition_table_get_partition_info(size "--partition-name voice_data" "size")
partition_table_get_partition_info(offset "--partition-name voice_data" "offset")

if("${size}" AND "${offset}")
    esptool_py_flash_to_partition(flash "voice_data" "${voice_data_image}")
else()
    set(message "Failed to find model in partition table file"
                "Please add a line(Name=voice_data, Type=data, Size=3890K) to the partition file.")
endif()
