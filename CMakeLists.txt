cmake_minimum_required(VERSION 3.5)

project(stplot LANGUAGES CXX C)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/deps/varloc/dwarves/cmake/modules")

add_subdirectory(deps/varloc)
add_subdirectory(deps/stlink-lib)
add_subdirectory(stplot-gui)
