set(COMPONENT_SRCS
    src/mn_process_commands.c
    src/model_path.c
    )

set(COMPONENT_ADD_INCLUDEDIRS 
    esp-tts/esp_tts_chinese/include
    include
    )

set(COMPONENT_REQUIRES
    json
    spiffs
    )

register_component()
if(IDF_TARGET STREQUAL "esp32")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese")
IF (IDF_VERSION_MAJOR GREATER 3)
    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_processor.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libwakenet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libmultinet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr)
ENDIF (IDF_VERSION_MAJOR GREATER 3)
    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        wakenet
        multinet
        dl_lib
        c_speech_features
        hilexin_wn5
        hilexin_wn5X2
        hilexin_wn5X3
        nihaoxiaozhi_wn5
        nihaoxiaozhi_wn5X2
        nihaoxiaozhi_wn5X3
        nihaoxiaoxin_wn5X3
        customized_word_wn5
        multinet2_ch
        esp_audio_processor 
        esp_audio_front_end
        esp_tts_chinese 
        voice_set_xiaole
        voice_set_template
        "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32s2")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese")
    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        esp_tts_chinese_esp32s2 
        voice_set_xiaole_esp32s2
        voice_set_template_esp32s2
         "-Wl,--end-group")
elseif(${IDF_TARGET} STREQUAL "esp32s3")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3")
    target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese")

    add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_processor.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libwakenet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libmultinet.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr)
    add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/esp32s3/libhufzip.a" PRIV_REQUIRES esp-sr)

    target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
        wakenet
        hufzip
        dl_lib
        c_speech_features
        esp_audio_front_end
        esp_audio_processor
        multinet
        esp_tts_chinese_esp32s3
        voice_set_xiaole_esp32s3
        "-Wl,--end-group")

        set(MVMODEL_EXE ${COMPONENT_PATH}/model/movemodel.py)
        add_custom_command(
             OUTPUT ${COMPONENT_DIR}/model/target/_MODEL_INFO_
             COMMENT "Running move model..."
             COMMAND python ${MVMODEL_EXE} -d1 ${PROJECT_DIR} -d2 ${COMPONENT_PATH}
             DEPENDS ${COMPONENT_DIR}/model/
             VERBATIM)

        add_custom_target(model DEPENDS ${COMPONENT_DIR}/model/target/_MODEL_INFO_)
        add_dependencies(${COMPONENT_LIB} model)
    if(CONFIG_MODEL_IN_SPIFFS AND CONFIG_USE_WAKENET)
        spiffs_create_partition_image(model ${COMPONENT_DIR}/model/target FLASH_IN_PROJECT DEPENDS ${COMPONENT_DIR}/model/target/_MODEL_INFO_)
    endif()
endif()
