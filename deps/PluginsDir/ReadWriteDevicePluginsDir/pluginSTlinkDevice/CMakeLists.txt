cmake_minimum_required(VERSION 3.5)

# create plugin and app
set(PLUGIN_NAME pluginSTlinkDevice)

project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#build stlink library
set(BUILD_STATIC_STLINK OFF)

add_subdirectory(stlink-lib)


# BEGIN PLUGIN
add_library(${PLUGIN_NAME} SHARED
    ${PATH_INTERFACE}/readwritedevice.h
    stlinkdevice.h stlinkdevice.cpp
    pluginSTlink.h
)

target_include_directories(${PLUGIN_NAME} PRIVATE stlink-lib/stlink/inc
    stlink-lib/stlink/src/stlink-lib
    ${CMAKE_BINARY_DIR}/stlink/inc
    ${LIBUSB_INCLUDE_DIR}
)

target_link_libraries(${PLUGIN_NAME} PRIVATE stlink-shared)

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_PATH}
)

if (WIN32)
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_PATH})
endif()
# END PLUGIN
