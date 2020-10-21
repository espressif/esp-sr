
set(COMPONENT_ADD_INCLUDEDIRS
    ./esp_tts_chinese/include
    )

register_component()

target_link_libraries(${COMPONENT_TARGET} INTERFACE "-L ${CMAKE_CURRENT_SOURCE_DIR}/esp_tts_chinese")
target_link_libraries(${COMPONENT_TARGET} INTERFACE
    esp_tts_chinese 
    voice_set_xiaole 
    voice_set_template
    )

