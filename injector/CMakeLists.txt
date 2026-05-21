cmake_minimum_required(VERSION 4.2)
project(NotOnArm64_Injector)

add_executable(injector main.c)

install(TARGETS injector)