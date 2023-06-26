cmake_minimum_required(VERSION 3.5)

project(stplot LANGUAGES CXX C)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

add_subdirectory(deps)
add_subdirectory(stplot-gui/)
