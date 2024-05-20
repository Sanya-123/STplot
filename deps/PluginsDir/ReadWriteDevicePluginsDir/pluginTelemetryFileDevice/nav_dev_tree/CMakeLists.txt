cmake_minimum_required(VERSION 3.5)

project(nav_dev_tree LANGUAGES C)

add_executable(nav_dev_tree main.c link.ld)

target_link_options(nav_dev_tree PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/link.ld)

set_target_properties(nav_dev_tree PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_PATH}
    RUNTIME_OUTPUT_NAME nav_dev_tree.elf
)

install(TARGETS nav_dev_tree
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
