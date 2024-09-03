cmake_minimum_required(VERSION 3.5)

project(nav_dev_tree LANGUAGES C)

add_executable(nav_dev_tree main.c link.ld)

target_link_options(nav_dev_tree PRIVATE -T${CMAKE_CURRENT_LIST_DIR}/link.ld)
if (WIN32)
    target_link_options(nav_dev_tree PRIVATE -nodefaultlibs)
endif()

target_include_directories(nav_dev_tree PRIVATE ${CMAKE_CURRENT_LIST_DIR}/PEK_Libs/inc)

set_target_properties(nav_dev_tree PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${PLUGINS_PATH}
    RUNTIME_OUTPUT_NAME nav_dev_tree.elf
)

if (WIN32)
    if(PLUGINS_PATH)
        add_custom_target(copy_nav_dev_tree ALL DEPENDS nav_dev_tree
            COMMAND ${CMAKE_COMMAND} -E copy ${PLUGINS_PATH}/nav_dev_tree.elf.exe ${CMAKE_BINARY_DIR}/stplot/plugins/nav_dev_tree.elf
        )
    endif()
endif()

install(TARGETS nav_dev_tree
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
