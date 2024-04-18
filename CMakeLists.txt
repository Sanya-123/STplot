cmake_minimum_required(VERSION 3.5)

project(stplot LANGUAGES CXX C)

#set(CMAKE_AUTOUIC ON)
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/deps/varloc/dwarves/cmake/modules")

set(PATH_COMMON_QT_LIBS "${CMAKE_CURRENT_LIST_DIR}/deps/commonQtLibs")

if (WIN32)
    #libs for windows
    include_directories(${CMAKE_SOURCE_DIR}/windowsLibs)
    set(ZLIB_LIBRARY zlib)
    set(ZLIB_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/windowsLibs/zlib)
    #set(CMAKE_LIBRARY_PATH ${CMAKE_SOURCE_DIR}/windowsLibs/zlib)
    add_subdirectory(windowsLibs/zlib)
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    add_subdirectory(deps/varloc)
endif()

add_subdirectory(deps/stlink-lib)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_subdirectory(deps/QtnProperty)
add_subdirectory(stplot-gui)

#generate runnable image
if (WIN32)
    #make image copy correct files
else()
    add_custom_target(deployment DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui ${CMAKE_CURRENT_BINARY_DIR}/deployment/stplot-gui
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lib/*.so ${CMAKE_CURRENT_BINARY_DIR}/deployment/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/plugins ${CMAKE_CURRENT_BINARY_DIR}/deployment/plugins
    )
endif()

