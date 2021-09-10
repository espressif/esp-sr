set(COMPONENT_SRCS
    speech_command_recognition/mn_process_commands.c
    )

set(COMPONENT_ADD_INCLUDEDIRS 
    lib/include
    wake_word_engine/include
    speech_command_recognition/include
    acoustic_algorithm/include
    esp-tts/esp_tts_chinese/include
    audio_front_end/include
    )

set(COMPONENT_REQUIRES
    json
    )

register_component()

target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/lib")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/wake_word_engine")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/speech_command_recognition")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/acoustic_algorithm")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp-tts/esp_tts_chinese")
target_link_libraries(${COMPONENT_TARGET} "-L ${CMAKE_CURRENT_SOURCE_DIR}/audio_front_end")

IF (IDF_VER MATCHES "v4.")
add_prebuilt_library(esp_audio_processor "${CMAKE_CURRENT_SOURCE_DIR}/acoustic_algorithm/libesp_audio_processor.a" PRIV_REQUIRES esp-sr) 
add_prebuilt_library(wakenet "${CMAKE_CURRENT_SOURCE_DIR}/lib/libwakenet.a" PRIV_REQUIRES esp-sr) 
ENDIF (IDF_VER MATCHES "v4.")
add_prebuilt_library(multinet "${CMAKE_CURRENT_SOURCE_DIR}/lib/libmultinet.a" PRIV_REQUIRES esp-sr) 

add_prebuilt_library(esp_audio_front_end "${CMAKE_CURRENT_SOURCE_DIR}/audio_front_end/libesp_audio_front_end.a" PRIV_REQUIRES esp-sr) 

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
    hilexin_wn7
    xiaoaitongxue_wn7
    xiaoaitongxue_wn7_q8
    alexa_wn7
    alexa_wn7_q8
    alexa_wn8
    customized_word_wn7
    dl_lib_esp32s3
    c_speech_features
    esp_audio_front_end
    esp_audio_processor
    multinet
    multinet5_en
    multinet5_en_q8
    multinet3_ch
    "-Wl,--end-group")
endif()
