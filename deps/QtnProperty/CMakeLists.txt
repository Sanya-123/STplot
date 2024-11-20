cmake_minimum_required(VERSION 3.5)
project(QtnProperty LANGUAGES CXX)

find_package(Qt5 COMPONENTS Widgets Script REQUIRED)

file(GLOB_RECURSE SRCS "QtnProperty/QtnProperty/*.cpp")
file(GLOB_RECURSE HEADERS "QtnProperty/QtnProperty/*.h")

add_library(QtnProperty SHARED ${SRCS} ${HEADERS} ./QtnProperty.qrc)

target_link_libraries(QtnProperty PRIVATE Qt5::Widgets Qt5::Script)

target_include_directories(QtnProperty PRIVATE ./QtnProperty ./QtnProperty/QtnProperty)

set_target_properties(QtnProperty PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

