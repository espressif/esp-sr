if(IDF_TARGET STREQUAL "esp32")
    set(include_dirs
        src/include
        esp-tts/esp_tts_chinese/include
        include/esp32
        )
    set(srcs
        src/model_path.c
        src/esp_mn_speech_commands.c
        src/esp_process_sdkconfig.c
        )

    set(requires
        json
        spiffs
        )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires}
                       PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32")
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_processor.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libwakenet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libmultinet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_front_end.a" PRIV_REQUIRES ${COMPONENT_NAME})

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        multinet
        dl_lib
        c_speech_features
        wakeword_model
        multinet2_ch
        esp_audio_processor
        esp_audio_front_end
        esp_tts_chinese
        voice_set_xiaole
        wakenet
        "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(include_dirs
        src/include
        esp-tts/esp_tts_chinese/include
        include/esp32s3
        )
    set(srcs
        src/model_path.c
        src/esp_mn_speech_commands.c
        src/esp_process_sdkconfig.c
        )

    set(requires
        json
        spiffs
        )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires}
                       PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32s3")

    add_prebuilt_library(flite_g2p "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libflite_g2p.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_processor.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libwakenet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libmultinet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_front_end.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libhufzip.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(nsnet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libnsnet.a" PRIV_REQUIRES ${COMPONENT_NAME})

    idf_component_get_property(esp_dsp_lib espressif__esp-dsp COMPONENT_LIB)

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        hufzip
        dl_lib
        fst
        c_speech_features
        $<TARGET_FILE:${esp_dsp_lib}>
        esp_audio_front_end
        esp_audio_processor
        multinet
        flite_g2p
        esp_tts_chinese
        voice_set_xiaole
        nsnet
        wakenet
        "-Wl,--end-group")

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

    partition_table_get_partition_info(size "--partition-name model" "size")
    partition_table_get_partition_info(offset "--partition-name model" "offset")

    if("${size}" AND "${offset}")
        esptool_py_flash_to_partition(flash "model" "${image_file}")
    else()
        set(message "Failed to find model in partition table file"
                    "Please add a line(Name=model, Size>recommended size in log) to the partition file.")
    endif()
elseif(${IDF_TARGET} STREQUAL "esp32p4")
    set(include_dirs
        src/include
        esp-tts/esp_tts_chinese/include
        include/esp32p4
        )
    set(srcs
        src/model_path.c
        src/esp_mn_speech_commands.c
        src/esp_process_sdkconfig.c
        )

    set(requires
        json
        spiffs
        )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS ${include_dirs}
                       REQUIRES ${requires}
                       PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32p4")

    add_prebuilt_library(flite_g2p "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libflite_g2p.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libesp_audio_processor.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libwakenet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libmultinet.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libesp_audio_front_end.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libhufzip.a" PRIV_REQUIRES ${COMPONENT_NAME})
    add_prebuilt_library(nsnet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32p4/libnsnet.a" PRIV_REQUIRES ${COMPONENT_NAME})

    idf_component_get_property(esp_dsp_lib espressif__esp-dsp COMPONENT_LIB)

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        hufzip
        dl_lib
        fst
        c_speech_features
        $<TARGET_FILE:${esp_dsp_lib}>
        esp_audio_front_end
        esp_audio_processor
        multinet
        flite_g2p
        esp_tts_chinese
        voice_set_xiaole
        wakenet
        nsnet
        "-Wl,--end-group")

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

    partition_table_get_partition_info(size "--partition-name model" "size")
    partition_table_get_partition_info(offset "--partition-name model" "offset")

    if("${size}" AND "${offset}")
        esptool_py_flash_to_partition(flash "model" "${image_file}")
    else()
        set(message "Failed to find model in partition table file"
                    "Please add a line(Name=model, Size>recommended size in log) to the partition file.")
    endif()
elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(requires
    spiffs
    )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS .
                       INCLUDE_DIRS  esp-tts/esp_tts_chinese/include
                       REQUIRES ${requires}
                       PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32s2")
    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-Wl,--start-group"
        esp_tts_chinese
        voice_set_xiaole
        "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32c3")
    set(requires
    spiffs
    )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS .
                   INCLUDE_DIRS  esp-tts/esp_tts_chinese/include
                   REQUIRES ${requires}
                   PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32c3")
    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-Wl,--start-group"
        esp_tts_chinese
        voice_set_xiaole
        "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32c6")
    set(requires
    spiffs
    )

    IF (IDF_VERSION_MAJOR GREATER 4)
        list(APPEND requires esp_partition)
    ENDIF (IDF_VERSION_MAJOR GREATER 4)

    idf_component_register(SRCS .
                   INCLUDE_DIRS  esp-tts/esp_tts_chinese/include
                   REQUIRES ${requires}
                   PRIV_REQUIRES spi_flash)

    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32c6")
    target_link_libraries(${COMPONENT_TARGET} INTERFACE "-Wl,--start-group"
        esp_tts_chinese
        voice_set_xiaole
        "-Wl,--end-group")
endif()
