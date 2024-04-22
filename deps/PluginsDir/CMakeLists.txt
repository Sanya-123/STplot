cmake_minimum_required(VERSION 3.5)

set(PLUGINS_PATH ${CMAKE_BINARY_DIR}/stplot-gui/plugins)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PATH_INTERFACE ${PATH_STPLOT_COMMON})

include_directories(${PATH_INTERFACE})
add_subdirectory(PlotPluginsDir)
add_subdirectory(VarLoaderPluginsDir)
