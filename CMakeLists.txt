cmake_minimum_required(VERSION 4.2)
project(NotOnArm64)

include(ExternalProject)
ExternalProject_Add(
    NotOnArm64_Launcher
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/launcher
    BUILD_ALWAYS true
    CMAKE_ARGS
        -A x64
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)
ExternalProject_Add(
    NotOnArm64_Payload_32
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/payload
    BUILD_ALWAYS true
    CMAKE_ARGS
        -A Win32
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)
ExternalProject_Add(
    NotOnArm64_Payload_64
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/payload
    BUILD_ALWAYS true
    CMAKE_ARGS
        -A x64
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
)

add_custom_target(NotOnArm64 ALL DEPENDS NotOnArm64_Launcher NotOnArm64_Payload_32 NotOnArm64_Payload_64)

ExternalProject_Get_Property(NotOnArm64_Launcher INSTALL_DIR)
add_custom_command(
    TARGET NotOnArm64 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${INSTALL_DIR}/bin/NotOnArm64_Launcher.exe" "${CMAKE_CURRENT_BINARY_DIR}/NotOnArm64_Launcher.exe"
)
ExternalProject_Get_Property(NotOnArm64_Payload_32 INSTALL_DIR)
add_custom_command(
    TARGET NotOnArm64 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${INSTALL_DIR}/bin/NotOnArm64_Payload.dll" "${CMAKE_CURRENT_BINARY_DIR}/NotOnArm64_Payload.32.dll"
)
ExternalProject_Get_Property(NotOnArm64_Payload_64 INSTALL_DIR)
add_custom_command(
    TARGET NotOnArm64 POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${INSTALL_DIR}/bin/NotOnArm64_Payload.dll" "${CMAKE_CURRENT_BINARY_DIR}/NotOnArm64_Payload.64.dll"
)