cmake_minimum_required(VERSION 3.25)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(${CMAKE_CURRENT_LIST_DIR}/../config.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/../external/pico-sdk/pico_sdk_init.cmake)

project(lab2b)

pico_sdk_init()

add_subdirectory(src)
