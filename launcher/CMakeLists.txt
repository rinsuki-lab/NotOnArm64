cmake_minimum_required(VERSION 4.2)
project(NotOnArm64_Launcher)

add_executable(NotOnArm64_Launcher WIN32 main.c)

install(TARGETS NotOnArm64_Launcher)