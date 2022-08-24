if(IDF_TARGET STREQUAL "esp32")
    set(COMPONENT_ADD_INCLUDEDIRS 
	src/include
        esp-tts/esp_tts_chinese/include
        include/esp32
        )
    set(COMPONENT_SRCS
        src/model_path.c
        src/esp_mn_speech_commands.c
        src/esp_process_sdkconfig.c
        )

    set(COMPONENT_REQUIRES
        json
        spiffs
        )

    register_component()

    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32")
IF (IDF_VERSION_MAJOR GREATER 3)
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_processor.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libwakenet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libmultinet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr)
ENDIF (IDF_VERSION_MAJOR GREATER 3)
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
elseif(${IDF_TARGET} STREQUAL "esp32s2")
    set(COMPONENT_ADD_INCLUDEDIRS 
        esp-tts/esp_tts_chinese/include
        include/esp32
	src/include
        )
    set(COMPONENT_SRCS
        src/esp_process_sdkconfig.c
        src/esp_mn_speech_commands.c
        src/model_path.c
        )

    set(COMPONENT_REQUIRES
        json
        spiffs
        )

    register_component()
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32s2")
    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        esp_tts_chinese
        voice_set_xiaole
         "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32s3")
    set(COMPONENT_ADD_INCLUDEDIRS 
	src/include
        esp-tts/esp_tts_chinese/include
        include/esp32s3
        )
    set(COMPONENT_SRCS
        src/model_path.c
        src/esp_mn_speech_commands.c
        src/esp_process_sdkconfig.c
        )

    set(COMPONENT_REQUIRES
        json
        spiffs
        )

    register_component()
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese/esp32s3")

    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_processor.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libwakenet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libmultinet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libhufzip.a" PRIV_REQUIRES esp-sr)

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        hufzip
        dl_lib
	esp-dsp
        c_speech_features
        esp_audio_front_end
        esp_audio_processor
        multinet
        esp_tts_chinese
        voice_set_xiaole
        wakenet
        "-Wl,--end-group")

    set(MVMODEL_EXE ${COMPONENT_PATH}/model/movemodel.py)
    add_custom_command(
         OUTPUT ${COMPONENT_DIR}/model/target/_MODEL_INFO_
         COMMENT "Running move model..."
         COMMAND python ${MVMODEL_EXE} -d1 ${PROJECT_DIR} -d2 ${COMPONENT_PATH}
         DEPENDS ${COMPONENT_DIR}/model/
         VERBATIM)

    add_custom_target(model)
    add_dependencies(${COMPONENT_LIB} model)

    idf_build_get_property(idf_path IDF_PATH)
    set(spiffsgen_py ${PYTHON} ${idf_path}/components/spiffs/spiffsgen.py)

    get_filename_component(base_dir_full_path ${COMPONENT_DIR}/model/target/ ABSOLUTE)

    partition_table_get_partition_info(size "--partition-name model" "size")
    partition_table_get_partition_info(offset "--partition-name model" "offset")
    partition_table_get_partition_info(offset "--partition-name model" "offset")

    if("${size}" AND "${offset}" AND CONFIG_MODEL_IN_SPIFFS AND CONFIG_USE_WAKENET)
        set(image_file ${CMAKE_BINARY_DIR}/model.bin)

        if(CONFIG_SPIFFS_USE_MAGIC)
            set(use_magic "--use-magic")
        endif()

        if(CONFIG_SPIFFS_USE_MAGIC_LENGTH)
            set(use_magic_len "--use-magic-len")
        endif()

        if(CONFIG_SPIFFS_FOLLOW_SYMLINKS)
            set(follow_symlinks "--follow-symlinks")
        endif()

        # Execute SPIFFS image generation; this always executes as there is no way to specify for CMake to watch for
        # contents of the base dir changing.
        add_custom_target(spiffs_model_bin ALL
            COMMAND ${spiffsgen_py} ${size} ${base_dir_full_path} ${image_file}
            --page-size=${CONFIG_SPIFFS_PAGE_SIZE}
            --obj-name-len=${CONFIG_SPIFFS_OBJ_NAME_LEN}
            --meta-len=${CONFIG_SPIFFS_META_LENGTH}
            ${follow_symlinks}
            ${use_magic}
            ${use_magic_len}
            DEPENDS ${COMPONENT_DIR}/model/target/_MODEL_INFO_
            )

        set_property(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" APPEND PROPERTY
            ADDITIONAL_MAKE_CLEAN_FILES
            ${image_file})

        idf_component_get_property(main_args esptool_py FLASH_ARGS)
        idf_component_get_property(sub_args esptool_py FLASH_SUB_ARGS)
        # Last (optional) parameter is the encryption for the target. In our
        # case, spiffs is not encrypt so pass FALSE to the function.
        esptool_py_flash_target(model-flash "${main_args}" "${sub_args}" ALWAYS_PLAINTEXT)
        esptool_py_flash_to_partition(model-flash "model" "${image_file}")

        add_dependencies(model-flash spiffs_model_bin)

        esptool_py_flash_to_partition(flash "model" "${image_file}")
        add_dependencies(flash spiffs_model_bin)
    else()
        set(message "Failed to create SPIFFS image for partition 'model'. "
                    "Check project configuration if using the correct partition table file.")
    endif()
endif()
