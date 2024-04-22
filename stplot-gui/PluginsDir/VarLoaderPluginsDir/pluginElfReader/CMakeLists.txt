cmake_minimum_required(VERSION 3.5)

# create plugin and app
set(PLUGIN_NAME pluginElfReader)

project(${PLUGIN_NAME} LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# BEGIN PLUGIN
if (WIN32)
    add_library(${PLUGIN_NAME} SHARED
        ${PATH_INTERFACE}/varreaderinterface.h
        elfreader.h elfreader.cpp
    )
else()
    add_subdirectory(varloc)
    add_library(${PLUGIN_NAME} SHARED
        ${PATH_INTERFACE}/varreaderinterface.h
        elfreader.h elfreader.cpp
        varloc/varloc.h
        varloc/varloc.c
    )
endif()

target_include_directories(${PLUGIN_NAME} PRIVATE varloc/dwarves varloc)
if (UNIX)
    target_link_libraries(${PLUGIN_NAME} PRIVATE dwarves)
endif()

set_target_properties(${PLUGIN_NAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${PLUGINS_PATH}
)

if (WIN32)
    install(TARGETS ${PLUGIN_NAME} DESTINATION ${PLUGINS_PATH})
endif()

# END PLUGIN
