cmake_minimum_required(VERSION 4.2)
project(NotOnArm64)

include(ExternalProject)
ExternalProject_Add(
    NotOnArm64_Injector
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/injector
    CMAKE_ARGS "-DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>"
)

add_custom_target(NotOnArm64 ALL DEPENDS NotOnArm64_Injector)
ExternalProject_Get_Property(NotOnArm64_Injector INSTALL_DIR)
add_custom_command(
    TARGET NotOnArm64 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${INSTALL_DIR}/bin/injector.exe" "${CMAKE_CURRENT_BINARY_DIR}/injector.exe"
)