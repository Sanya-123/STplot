cmake_minimum_required(VERSION 3.5)

# create plugin and app
set(PLUGIN_NAME pluginTestingScript)

project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Script)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Script)

# BEGIN PLUGIN
add_library(${PLUGIN_NAME} SHARED
    ${PATH_INTERFACE}/settingsabstract.h
    ${PATH_INTERFACE}/plotinterface.h
    testingscript.h testingscript.cpp testingscript.ui
    scriptexecutor.h scriptexecutor.cpp
    codeeditor.h codeeditor.cpp
    highlighter.h highlighter.cpp
)

target_link_libraries(${PLUGIN_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Script)

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_PATH}
)

if (WIN32)
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_PATH})
endif()
# END PLUGIN
