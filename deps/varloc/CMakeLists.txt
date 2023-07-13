cmake_minimum_required(VERSION 3.5)
#Global Variable Memmory Locator
project(
  varloc
  VERSION 1.0
  LANGUAGES C
)


set(BUILD_SHARED_LIBS OFF)
add_subdirectory(dwarves/)


add_library(varloc STATIC varloc.c varloc.h)
target_include_directories(varloc PUBLIC dwarves)
target_link_libraries(varloc dwarves)
