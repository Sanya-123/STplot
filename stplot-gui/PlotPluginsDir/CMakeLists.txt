cmake_minimum_required(VERSION 3.5)

set(PLUGINS_PATH ${CMAKE_CURRENT_BINARY_DIR}/../plugins)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network SerialPort PrintSupport REQUIRED)

set(PATH_PLOT_INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../modules/ViewManager)

include_directories(${PATH_PLOT_INTERFACE})
link_libraries(Qt5::Widgets Qt5::Network Qt5::SerialPort Qt5::PrintSupport)
add_subdirectory(pluginSimpleGraph)
