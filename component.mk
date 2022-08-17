COMPONENT_ADD_INCLUDEDIRS := include/$(IDF_TARGET) \
                             esp-tts/esp_tts_chinese/include \
			     src/include \

COMPONENT_SRCDIRS := src


LIB_FILES := $(shell ls $(COMPONENT_PATH)/lib/$(IDF_TARGET)/lib*.a) \
             $(shell ls $(COMPONENT_PATH)/esp-tts/esp_tts_chinese/esp32/lib*.a)

LIBS := $(patsubst lib%.a,-l%,$(LIB_FILES))

COMPONENT_ADD_LDFLAGS +=  -L$(COMPONENT_PATH)/lib/$(IDF_TARGET) \
                          -L$(COMPONENT_PATH)/esp-tts \
                          $(LIBS)
