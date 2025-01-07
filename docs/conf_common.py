from esp_docs.conf_docs import *  # noqa: F403,F401

languages = ['en', 'zh_CN']
idf_targets = ['esp32', 'esp32s3', 'esp32p4']

extensions += ['sphinx_copybutton',
               'sphinxcontrib.wavedrom',
               ]

# link roles config
github_repo = 'espressif/esp-sr'

# context used by sphinx_idf_theme
html_context['github_user'] = 'espressif'
html_context['github_repo'] = 'esp-sr'

html_static_path = ['../_static']

# Extra options required by sphinx_idf_theme
project_slug = 'esp-sr'

# Contains info used for constructing target and version selector
# Can also be hosted externally, see esp-idf for example
versions_url = './_static/docs_version.js'

# Final PDF filename will contains target and version
pdf_file_prefix = u'esp-sr'

# add Tracking id for Google Analytics

google_analytics_id = 'G-LCZ2R2P0F4'
