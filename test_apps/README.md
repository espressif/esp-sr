
Steps to run these cases:

- Build
  - . ${IDF_PATH}/export.sh
  - pip install idf_build_apps
  - python test_apps/build_apps.py test_apps -t esp32s3

- Test
  - pip install -r test_apps/requirement.txt
  - pytest test_apps --target esp32s3