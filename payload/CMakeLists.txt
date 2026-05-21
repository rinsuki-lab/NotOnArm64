cmake_minimum_required(VERSION 4.2)
project(NotOnArm64_Payload)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(../third_party/minhook minhook)

add_library(
    NotOnArm64_Payload
    SHARED
    main.cpp
    utils.cpp
    hook_create_process.cpp
    hook_hide_arm64.cpp
)
target_link_libraries(NotOnArm64_Payload PRIVATE minhook Pathcch.lib)

install(TARGETS NotOnArm64_Payload)