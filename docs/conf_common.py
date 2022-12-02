# -*- coding: utf-8 -*-
#
# Common (non-language-specific) configuration for Sphinx
#
# This file is imported from a language-specific conf.py (ie en/conf.py or
# zh_CN/conf.py)

from __future__ import print_function, unicode_literals

import os.path

#ESP_DOCS_PATH = os.environ['ESP_DOCS_PATH']

try:
    from esp_docs.conf_docs import *  # noqa: F403,F401
except ImportError:
    import os
    import sys
    sys.path.insert(0, os.path.abspath(ESP_DOCS_PATH))
    from conf_docs import *  # noqa: F403,F401

ESP32_DOCS = ['audio_front_end/README.rst',
              'wake_word_engine/README.rst',
              'wake_word_engine/ESP_Wake_Words_Customization.rst',
              'speech_command_recognition/README.rst',
              'flash_model/README.rst',
              'audio_front_end/Espressif_Microphone_Design_Guidelines.rst',
              'test_report/README.rst',
              'benchmark/README.rst',
            ]

# format: {tag needed to include: documents to included}, tags are parsed from sdkconfig and peripheral_caps.h headers
conditional_include_dict = {
                            'esp32':ESP32_DOCS,
                            }

extensions += ['sphinx_copybutton',
               # Note: order is important here, events must
               # be registered by one extension before they can be
               # connected to another extension
               'esp_docs.esp_extensions.dummy_build_system',
               'esp_docs.esp_extensions.run_doxygen',
               ]

# link roles config
github_repo = 'espressif/esp-sr'

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'
html_context['github_repo'] = 'esp-sr'

idf_targets = ['esp32', 'esp32s2', 'esp32s3']
languages = ['en', 'zh_CN']

google_analytics_id = os.environ.get('CI_GOOGLE_ANALYTICS_ID', None)

project_homepage = 'https://github.com/espressif/esp-sr'

html_static_path = ['../_static']

# Extra options required by sphinx_idf_theme
project_slug = 'esp-sr'

versions_url = './_static/js/at_versions.js'

# Final PDF filename will contains target and version
pdf_file_prefix = u'esp-sr'
