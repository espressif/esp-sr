# This is the project CMakeLists.txt file for the test subproject
cmake_minimum_required(VERSION 3.5)

# Include the components directory of the main application:
#
set(EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/tools/unit-test-app/components"
                         "../../../esp-sr")

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32c5_test)
