cmake_minimum_required(VERSION 3.5)

project(stplot LANGUAGES CXX C)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/deps/varloc/dwarves/cmake/modules")

set(PATH_COMMON_QT_LIBS "${CMAKE_CURRENT_LIST_DIR}/deps/commonQtLibs")
add_subdirectory(deps/varloc)
add_subdirectory(deps/stlink-lib)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_subdirectory(deps/QtnProperty)
add_subdirectory(stplot-gui)
