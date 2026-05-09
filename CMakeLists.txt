if((${IDF_TARGET} STREQUAL "esp32s3") OR (${IDF_TARGET} STREQUAL "esp32p4") OR (${IDF_TARGET} STREQUAL "esp32") OR (${IDF_TARGET} STREQUAL "esp32c3") OR (${IDF_TARGET} STREQUAL "esp32c5") OR (${IDF_TARGET} STREQUAL "esp32c6") OR  (${IDF_TARGET} STREQUAL "esp32s2") OR (${IDF_TARGET} STREQUAL "esp32s31"))
    set(TARGET_LIB_PATH "${IDF_TARGET}")
    
    if(CONFIG_IDF_TARGET_ESP32P4 AND IDF_VERSION_MAJOR EQUAL 5)
        if(CONFIG_ESP32P4_SELECTS_REV_LESS_V3 OR IDF_VERSION_MINOR LESS 5)
            set(TARGET_LIB_PATH "esp32p4_less_v3")
        endif()
    endif()

    if(CONFIG_ESP32P4_SELECTS_REV_LESS_V3 AND IDF_VERSION_MAJOR EQUAL 6)
        message(FATAL_ERROR "ESP-SR only support ESP32P4 rev less v3 on ESP-IDF v5.5 or earlier")
    endif()

    message(STATUS "CONFIG_ESP32P4_SELECTS_REV_LESS_V3 is ${CONFIG_ESP32P4_SELECTS_REV_LESS_V3}")
    message(STATUS "IDF_VERSION_MAJOR is ${IDF_VERSION_MAJOR}")
    message(STATUS "IDF_VERSION_MINOR is ${IDF_VERSION_MINOR}")
    message(STATUS "IDF_VERSION_PATCH is ${IDF_VERSION_PATCH}")
    message(STATUS "TARGET_LIB_PATH is set to: ${TARGET_LIB_PATH}")

    set(include_dirs
        "esp-tts/esp_tts_chinese/include"
        "include/${TARGET_LIB_PATH}"
        "src/include"
        )
        
    set(srcs
        "src/model_path.c"
        "src/esp_sr_debug.c"
        "src/esp_mn_speech_commands.c"
        "src/esp_process_sdkconfig.c"
        )

    set(requires
        spiffs
        cjson
        )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS ${srcs}
                        INCLUDE_DIRS ${include_dirs}
                        REQUIRES ${requires}
                        PRIV_REQUIRES spi_flash)
    
    target_link_libraries(${COMPONENT_TARGET} "-L \"${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}\"")
    target_link_libraries(${COMPONENT_TARGET} "-L \"${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/${IDF_TARGET}\"")
    add_prebuilt_library(dl_lib "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libdl_lib.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(c_speech_features "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libc_speech_features.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libesp_audio_processor.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libesp_audio_front_end.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_tts_chinese "${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/${IDF_TARGET}/libesp_tts_chinese.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(voice_set_xiaole "${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/${IDF_TARGET}/libvoice_set_xiaole.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(fst "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libfst.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(flite_g2p "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libflite_g2p.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libmultinet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libhufzip.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(vadnet "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libvadnet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libwakenet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(nsnet "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libnsnet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    idf_component_get_property(esp_dsp_lib espressif__esp-dsp COMPONENT_LIB)
    idf_component_get_property(dl_fft_lib espressif__dl_fft COMPONENT_LIB)
    idf_component_get_property(cjson_lib espressif__cjson COMPONENT_LIB)

    set(sr_libs 
        $<TARGET_FILE:${esp_dsp_lib}>
        $<TARGET_FILE:${dl_fft_lib}>
        $<TARGET_FILE:${cjson_lib}>
        dl_lib
        c_speech_features
        esp_audio_front_end
        esp_audio_processor
        esp_tts_chinese
        voice_set_xiaole
        fst
        flite_g2p
        hufzip
        multinet
        nsnet
        vadnet
        wakenet)

    if(${IDF_TARGET} STREQUAL "esp32")
        add_prebuilt_library(multinet2_ch "${CMAKE_CURRENT_SOURCE_DIR}/lib/${TARGET_LIB_PATH}/libmultinet2_ch.a" PRIV_REQUIRES ${COMPONENT_NAME})
        list(APPEND sr_libs multinet2_ch)
    endif()

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        ${sr_libs}
        "-Wl,--end-group")


endif()

# Add model partition and flash srmodels.bin
if(CONFIG_PARTITION_TABLE_CUSTOM)
    partition_table_get_partition_info(size "--partition-name model" "size")
    partition_table_get_partition_info(offset "--partition-name model" "offset")

    if("${size}" AND "${offset}")
        set(MVMODEL_EXE ${COMPONENT_PATH}/model/movemodel.py)
        idf_build_get_property(build_dir BUILD_DIR)
        set(image_file ${build_dir}/srmodels/srmodels.bin)

        add_custom_command(
            OUTPUT ${image_file}
            COMMENT "Move and Pack models..."
            COMMAND python ${MVMODEL_EXE} -d1 ${SDKCONFIG} -d2 ${COMPONENT_PATH} -d3 ${build_dir}
            DEPENDS ${SDKCONFIG}
            VERBATIM)

        add_custom_target(srmodels_bin ALL DEPENDS ${image_file})
        add_dependencies(flash srmodels_bin)
        esptool_py_flash_to_partition(flash "model" "${image_file}")
    else()
        set(message "Failed to find model in partition table file"
                    "Please add a line(Name=model) to the partition file if you want to use esp-sr models.")
    endif()
endif()
