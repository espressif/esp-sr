var DOCUMENTATION_VERSIONS = {
  DEFAULTS: { has_targets: false,
              supported_targets: [ "esp32s3" ]
            },
  VERSIONS: [
    { name: "latest", has_targets: true, supported_targets: [ "esp32", "esp32s3", "esp32p4" ] },
  ],
  IDF_TARGETS: [
     { text: "ESP32", value: "esp32"},
     { text: "ESP32-S3", value: "esp32s3"},
     { text: "ESP32-P4", value: "esp32p4"},
  ]
};
