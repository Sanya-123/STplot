cmake_minimum_required(VERSION 3.5)

project(stplot LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/deps/varloc/dwarves/cmake/modules")
set(PATH_COMMON_QT_LIBS "${CMAKE_CURRENT_LIST_DIR}/deps/commonQtLibs")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

if(WIN32)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/include)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/include)
    set(CMAKE_INSTALL_LIBDIR ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui)
    set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/install)
endif()

add_subdirectory(deps/STplotVarCommon)
set(PATH_STPLOT_COMMON ${CMAKE_SOURCE_DIR}/deps/STplotVarCommon)
if (WIN32)
    option(USE_OPENGL "Use OpenGL" OFF)
    option(BUILD_STATIC_STLINK "Build stlink static library" OFF)
else()
    option(USE_OPENGL "Use OpenGL" ON)
    option(BUILD_STATIC_STLINK "Build stlink static library" ON)
endif()

add_subdirectory(deps/stlink-lib)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
add_subdirectory(deps/QtnProperty)
#add_subdirectory(stplot-gui/modules/VarChannel)
add_subdirectory(deps/PluginsDir)
add_subdirectory(stplot-gui)

if(NOT QT_QMAKE_EXECUTABLE)
    set(QT_QMAKE_EXECUTABLE ${CMAKE_PREFIX_PATH}/bin/qmake)
endif()

#generate runnable image
if (WIN32)
    #make image copy correct files
else()
    add_custom_target(deployment_CP DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui ${CMAKE_CURRENT_BINARY_DIR}/deployment/stplot-gui
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/lib/*.so ${CMAKE_CURRENT_BINARY_DIR}/deployment/
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/plugins/ ${CMAKE_CURRENT_BINARY_DIR}/deployment/plugins
        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/plugins/* ${CMAKE_CURRENT_BINARY_DIR}/deployment/plugins/
    )
    if(CI_TOOLS_PATH)
        add_custom_target(deployment_Image DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui deployment_CP
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/resource/appicon.png ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui.png
            COMMAND QMAKE=${QT_QMAKE_EXECUTABLE} ${CI_TOOLS_PATH}/linuxdeploy-x86_64.AppImage --create-desktop-file -e ${CMAKE_CURRENT_BINARY_DIR}/stplot-gui/stplot-gui -i stplot-gui.png --appdir AppDir --plugin qt --output appimage
            COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/*.AppImage ${CMAKE_CURRENT_BINARY_DIR}/deployment/
        )
        add_custom_target(deployment DEPENDS deployment_Image deployment_CP)
    else()
        add_custom_target(deployment DEPENDS deployment_CP)
    endif()
endif()

#some libs
if (WIN32)
    install(TARGETS QtnProperty DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/lib)
#    add_custom_target(copyLibs DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/windowsLibs/libusb-cmake/libusb-1.0.a
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/windowsLibs/libusb-cmake/libusb-1.0.a ${CMAKE_CURRENT_BINARY_DIR}/lib/liblibusb.a
#    )
#    add_dependencies(stplot-gui copyLibs)
endif()

