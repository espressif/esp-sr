set(COMPONENT_SRCS
    speech_command_recognition/mn_process_commands.c
    model/model_path.c
    )

set(COMPONENT_ADD_INCLUDEDIRS 
    lib/include
    wake_word_engine/include
    speech_command_recognition/include
    acoustic_algorithm/include
    esp-tts/esp_tts_chinese/include
    audio_front_end/include
    model
    )

set(COMPONENT_REQUIRES
    json
    spiffs
    )

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/wake_word_engine")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/speech_command_recognition")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/acoustic_algorithm")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/audio_front_end")

add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/acoustic_algorithm/libesp_audio_processor.a" PRIV_REQUIRES esp-sr) 
add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/libwakenet.a" PRIV_REQUIRES esp-sr)
add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmultinet.a" PRIV_REQUIRES esp-sr) 
add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/audio_front_end/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr) 
add_prebuilt_library(hufzip "${CMAKE_CURRENT_SOURCE_DIR}/lib/libhufzip.a" PRIV_REQUIRES esp-sr) 

if(IDF_TARGET STREQUAL "esp32")
target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
    wakenet
    dl_lib_esp32
    c_speech_features
    hilexin_wn5
    hilexin_wn5X3
    customized_word_wn5
    multinet
    multinet1_en
    multinet2_ch
    esp_tts_chinese 
    voice_set_xiaole 
    voice_set_template
    esp_audio_front_end
    esp_audio_processor "-Wl,--end-group")
endif()

if(IDF_TARGET STREQUAL "esp32s2")
target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
    esp_tts_chinese_esp32s2 
    voice_set_xiaole_esp32s2
    voice_set_template_esp32s2
     "-Wl,--end-group")
endif()


if(IDF_TARGET STREQUAL "esp32s3")
target_link_libraries(${COMPONENT_TARGET} "-Wl,--start-group"
    wakenet
    hufzip
    dl_lib_esp32s3
    c_speech_features
    esp_audio_front_end
    esp_audio_processor
    multinet
    "-Wl,--end-group")
endif()

set(MVMODEL_EXE ${COMPONENT_PATH}/model/movemodel.py)

if(CONFIG_MODEL_IN_SPIFFS AND CONFIG_USE_WAKENET)
    add_custom_command(
         OUTPUT ${COMPONENT_DIR}/model/target/_MODEL_INFO_
         COMMENT "Running move model..."
         COMMAND python ${MVMODEL_EXE} -d1 ${PROJECT_DIR} -d2 ${COMPONENT_PATH}
         DEPENDS ${COMPONENT_DIR}/model/
         VERBATIM)

    add_custom_target(model DEPENDS ${COMPONENT_DIR}/model/target/_MODEL_INFO_)
    add_dependencies(${COMPONENT_LIB} model)

    spiffs_create_partition_image(model ${COMPONENT_DIR}/model/target FLASH_IN_PROJECT DEPENDS ${COMPONENT_DIR}/model/target/_MODEL_INFO_)
endif()