cmake_minimum_required(VERSION 3.24)

project(bluemetal)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

option(BLUEMETAL_DEBUGGING ON "Enables debugging commands in engine.")
option(BLUEMETAL_DEVELOPMENT ON "Enables development mode and splits modules into shared libraries.")

add_subdirectory("ThirdParty")
add_subdirectory("Source")