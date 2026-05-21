cmake_minimum_required(VERSION 4.2)
project(NotOnArm64_Payload)

add_subdirectory(../third_party/minhook minhook)

add_library(
    NotOnArm64_Payload
    SHARED
    main.cpp
    utils.cpp
    hook_create_process.cpp
)
target_link_libraries(NotOnArm64_Payload PRIVATE minhook Pathcch.lib)

install(TARGETS NotOnArm64_Payload)