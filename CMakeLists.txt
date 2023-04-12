cmake_minimum_required(VERSION 3.24)

project(BloodLust)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

option(BLOODLUST_DEBUGGING ON "Enables debugging commands in engine.")

add_subdirectory("ThirdParty")
add_subdirectory("Source")