cmake_minimum_required(VERSION 3.5)

project(stplot-gui VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        stplotwindow.cpp
        stplotwindow.h
        stplotwindow.ui
        widgets/VarLoader/varloader.h
        widgets/VarLoader/varloader.cpp
        widgets/VarLoader/varloader.ui
        widgets/VarLoader/varmodel.cpp
        widgets/VarLoader/varmodel.h
        widgets/ChannelsList/channelmodel.h
        widgets/ChannelsList/channelmodel.cpp
        widgets/ChannelsList/channels.h
        widgets/ChannelsList/channels.cpp
        widgets/ChannelsList/channels.ui
        widgets/ViewManager/viewmanager.h
        widgets/ViewManager/viewmanager.cpp
        widgets/ViewManager/viewmanager.ui
        modules/VarChannel/varchannel.h
        modules/VarChannel/varchannel.cpp
        modules/VarView/varview.cpp
        modules/VarView/varview.h
        modules/ViewManager/plotinterface.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(stplot-gui
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET stplot-gui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(stplot-gui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(stplot-gui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

set(VERSION_SHORT "3.5.2")
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/deps/qt-advanced-docking-system/cmake/modules ${CMAKE_MODULE_PATH})
option(BUILD_STATIC "Build the static library" ON)
add_subdirectory(${CMAKE_SOURCE_DIR}/deps/qt-advanced-docking-system/src qt-ads)

set(STLINK_PATH ../deps/stlink-lib/stlink)
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/deps/qt-advanced-docking-system/src
   ${CMAKE_SOURCE_DIR}/deps/varloc
    ${STLINK_PATH}/src/stlink-lib
    ${LIBUSB_INCLUDE_DIR}
    ${STLINK_PATH}/inc
    ${CMAKE_BINARY_DIR}/inc
    ${CMAKE_BINARY_DIR}/stlink/inc
    modules/VarModel
    modules/VarChannel
    modules/ViewManager
    widgets/VarLoader
    widgets/ChannelsList
    widgets/ViewManager/
  )


target_link_libraries(stplot-gui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    qtadvanceddocking
    xcb
    stlink-static
    varloc
#    elfio::elfio
)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    BUILD_TIME="${BUILD_TIME}"
    BUILD_DATE="${BUILD_DATE}"
    GIT_COMMIT="${GIT_COMMIT}"
    VERSION_MAJOR=${VERSION_MAJOR}
    VERSION_MINOR=${VERSION_MINOR}
    VERSION_BUILD=${SUBVERSION}
)


set_target_properties(stplot-gui PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS stplot-gui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(stplot-gui)
endif()
