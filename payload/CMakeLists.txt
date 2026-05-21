cmake_minimum_required(VERSION 4.2)
project(NotOnArm64_Payload)

add_library(NotOnArm64_Payload SHARED main.c)

install(TARGETS NotOnArm64_Payload)