cmake_minimum_required(VERSION 3.5)

project(stplot-gui VERSION 0.1 LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

message(STATUS "OpenGL = ${USE_OPENGL} ")
if(USE_OPENGL)
    set(OpenGL_GL_PREFERENCE GLVND)
    find_package(OpenGL)
    set(QT_OPENGL_PACKAGE OpenGL)
    add_definitions(-DQCUSTOMPLOT_USE_OPENGL)
    set(OPENGL_LIBS "Qt${QT_VERSION_MAJOR}::OpenGL")
endif()
find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets ${QT_OPENGL_PACKAGE} Network SerialPort PrintSupport Script Svg)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets ${QT_OPENGL_PACKAGE} Network SerialPort PrintSupport Script Svg)

set(PROJECT_SOURCES
        main.cpp
        stplotwindow.cpp
        stplotwindow.h
        stplotwindow.ui
        widgets/VarLoader/varloader.h
        widgets/VarLoader/varloader.cpp
        widgets/VarLoader/varloader.ui
        widgets/VarLoader/varfilter.cpp
        widgets/VarLoader/varfilter.h
        widgets/VarLoader/varmodel.cpp
        widgets/VarLoader/varmodel.h
        widgets/ChannelsList/channelmodel.h
        widgets/ChannelsList/channelmodel.cpp
        widgets/ChannelsList/channels.h
        widgets/ChannelsList/channels.cpp
        widgets/ChannelsList/channels.ui
        widgets/ChannelsList/comboboxdelegate.h
        widgets/ChannelsList/comboboxdelegate.cpp
        widgets/ChannelsList/chanaleitemdelegate.h
        widgets/ChannelsList/chanaleitemdelegate.cpp
        widgets/SettingsDialog/settingsdialog.cpp
        widgets/SettingsDialog/settingsdialog.h
        # widgets/plotsettingsviever.h
        # widgets/plotsettingsviever.cpp
        widgets/ViewManager/viewmanager.h
        widgets/ViewManager/viewmanager.cpp
        widgets/ViewManager/viewmanager.ui
        widgets/ChannelsList/chanalematheditor.h
        widgets/ChannelsList/chanalematheditor.cpp
        widgets/ChannelsList/chanalematheditor.ui
        widgets/ChannelsList/newchanaledialog.h
        widgets/ChannelsList/newchanaledialog.cpp
        widgets/ChannelsList/newchanaledialog.ui
        modules/ReadManager/readloop.h
        modules/ReadManager/readloop.cpp
        modules/ReadManager/readmanager.h
        modules/ReadManager/readmanager.cpp
        ${PATH_STPLOT_COMMON}/varchannel.h
        ${PATH_STPLOT_COMMON}/plotinterface.h
        ${PATH_STPLOT_COMMON}/plotwidgetabstract.h
        ${PATH_STPLOT_COMMON}/readwritedevice.h
        ${PATH_STPLOT_COMMON}/settingsabstract.h
        ${PATH_STPLOT_COMMON}/stplotpluginloader.h
        ${PATH_STPLOT_COMMON}/varreaderinterface.h
        ${PATH_COMMON_QT_LIBS}/qcustomplot.h
        ${PATH_COMMON_QT_LIBS}/qcustomplot.cpp

        widgets/DebugerWindow/debugerwindow.h widgets/DebugerWindow/debugerwindow.cpp widgets/DebugerWindow/debugerwindow.ui
        widgets/DebugerWindow/debugerfilterseditor.h widgets/DebugerWindow/debugerfilterseditor.cpp widgets/DebugerWindow/debugerfilterseditor.ui
        widgets/DebugerWindow/debugerfiltermodel.h widgets/DebugerWindow/debugerfiltermodel.cpp
        widgets/DebugerWindow/debugmessadge.h widgets/DebugerWindow/debugmessadge.cpp widgets/DebugerWindow/debugmessadge.ui
        widgets/DebugerWindow/debugerfilter.h
        widgets/Settings/settingswindow.h widgets/Settings/settingswindow.cpp widgets/Settings/settingswindow.ui

        #build styles maybe move it into libs
        ${CMAKE_SOURCE_DIR}/deps/styles/Qt-Advanced-Stylesheets/src/acss_globals.h
        ${CMAKE_SOURCE_DIR}/deps/styles/Qt-Advanced-Stylesheets/src/QtAdvancedStylesheet.h
        ${CMAKE_SOURCE_DIR}/deps/styles/Qt-Advanced-Stylesheets/src/QtAdvancedStylesheet.cpp
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

add_subdirectory(${CMAKE_SOURCE_DIR}/deps/qt-advanced-docking-system/src qt-ads)

add_custom_target(copyStyles ALL DEPENDS stplot-gui
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/deps/styles/Qt-Advanced-Stylesheets/styles ${CMAKE_CURRENT_BINARY_DIR}/styles
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PATH_STPLOT_COMMON}
    ${CMAKE_SOURCE_DIR}/deps/qt-advanced-docking-system/src
    ${CMAKE_BINARY_DIR}/inc
    ${CMAKE_BINARY_DIR}/stlink/inc
    modules
    ${PATH_STPLOT_COMMON}
    modules/ReadManager
    widgets
    widgets/VarLoader
    widgets/SettingsDialog
    widgets/ChannelsList
    widgets/ViewManager
    widgets/PlotWidget
    widgets/DebugerWindow
    widgets/DebugerWindow
    widgets/Settings
    ${CMAKE_SOURCE_DIR}/deps/styles/Qt-Advanced-Stylesheets/src
    ${PATH_COMMON_QT_LIBS}
    ${CMAKE_SOURCE_DIR}/deps/QtnProperty
    ${CMAKE_SOURCE_DIR}/deps
  )

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::SerialPort
    Qt${QT_VERSION_MAJOR}::PrintSupport
    Qt${QT_VERSION_MAJOR}::Script
    Qt${QT_VERSION_MAJOR}::Svg
    ${OPENGL_LIBS}
    ${OPENGL_LIBRARIES}
    qtadvanceddocking
    QtnProperty
    VarChannel
)

target_compile_definitions(${PROJECT_NAME} PRIVATE ACSS_STATIC)

if (UNIX)
    target_link_libraries(${PROJECT_NAME} PRIVATE xcb)
endif()

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/lib
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS ${PROJECT_NAME}
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(stplot-gui)
endif()
