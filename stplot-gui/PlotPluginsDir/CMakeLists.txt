cmake_minimum_required(VERSION 3.5)

set(PLUGINS_PATH ${CMAKE_CURRENT_BINARY_DIR}/../plugins)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Widgets Network SerialPort PrintSupport REQUIRED)

set(PATH_PLOT_INTERFACE ${CMAKE_CURRENT_LIST_DIR}/../modules)
set(PATH_VARCHANNEL ${CMAKE_CURRENT_LIST_DIR}/../modules/VarChannel)

include_directories(${PATH_PLOT_INTERFACE} ${PATH_COMMON_QT_LIBS} ${CMAKE_CURRENT_LIST_DIR}/../modules/VarChannel/ ${CMAKE_CURRENT_LIST_DIR}/../../deps/varloc)
link_libraries(Qt5::Widgets Qt5::Network Qt5::SerialPort Qt5::PrintSupport)

macro(subdirlist result curdir)
    file(GLOB children RELATIVE ${curdir} ${curdir}/*)
    set(dirlist "")
    foreach(child ${children})
        if(IS_DIRECTORY ${curdir}/${child})
            list(APPEND dirlist ${child})
        endif()
    endforeach()
    set(${result} ${dirlist})
endmacro()

subdirlist(SUBDIRS ${CMAKE_CURRENT_LIST_DIR})

foreach(subdir ${SUBDIRS})
    add_subdirectory(${subdir})
endforeach()
