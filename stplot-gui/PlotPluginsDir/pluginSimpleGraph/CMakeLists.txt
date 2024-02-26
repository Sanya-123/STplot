cmake_minimum_required(VERSION 3.5)

# create plugin and app
set(PLUGIN_NAME pluginSimpleGraph)

project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# BEGIN PLUGIN
add_library(${PLUGIN_NAME} SHARED
    ${PATH_PLOT_INTERFACE}/settingsabstract.h
    ${PATH_PLOT_INTERFACE}/plotinterface.h
    #TODO build var chanale as lib
    ${PATH_VARCHANNEL}/varchannel.h
    ${PATH_VARCHANNEL}/varchannel.cpp
    simplegraph.h simplegraph.cpp simplegraph.ui
    ${PATH_COMMON_QT_LIBS}/qcustomplot.h
    ${PATH_COMMON_QT_LIBS}/qcustomplot.cpp
)

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PLUGINS_PATH}"
)

target_compile_definitions(${PLUGIN_NAME} PRIVATE PLUGINFLASH_LIBRARY)
# END PLUGIN
