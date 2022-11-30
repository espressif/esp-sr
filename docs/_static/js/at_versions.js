var DOCUMENTATION_VERSIONS = {
    DEFAULTS: { has_targets: false,
                supported_targets: [ "esp32s3" ]
              },
    VERSIONS: [
        // latest
        { name: "latest", has_targets: true, supported_targets: [ "esp32", "esp32c2", "esp32c3" ] },

        // v2.4.0.0
        { name: "release-v2.4.0.0", has_targets: true, supported_targets: [ "esp32", "esp32c3" ]},

        // v2.3.0.0
        { name: "release-v2.3.0.0_esp32c3", has_targets: false, supported_targets: [ "esp32c3" ]},

        // v2.2.0.0
        { name: "release-v2.2.0.0_esp32c3", has_targets: false, supported_targets: [ "esp32c3" ]},
        { name: "release-v2.2.0.0_esp32", has_targets: false, supported_targets: [ "esp32", "esp32s2" ]},
        { name: "release-v2.2.0.0_esp8266", has_targets: false, supported_targets: [ "esp8266" ]},

        // v2.1.0.0
        { name: "release-v2.1.0.0_esp32", has_targets: false, supported_targets: [ "esp32" ]},
        { name: "release-v2.1.0.0_esp8266", has_targets: false, supported_targets: [ "esp8266" ]},
        { name: "release-v2.1.0.0_esp32s2", has_targets: false, supported_targets: [ "esp32s2" ]},
    ],
    IDF_TARGETS: [
       { text: "ESP32-C2 (ESP8684)", value: "esp32c2"},
       { text: "ESP32-C3", value: "esp32c3"},
       { text: "ESP32", value: "esp32"},
       { text: "ESP8266", value: "esp8266"},
       { text: "ESP32-S2", value: "esp32s2"},
    ]
};
