cmake_minimum_required(VERSION 3.24)

project(bluemetal)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMake/Modules")

option(BLUEMETAL_DEBUGGING OFF "Enables debugging commands in engine.")
option(BLUEMETAL_DEVELOPMENT ON "Enables development mode and splits modules into shared libraries.")


# Find Libraries
if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/ThirdParty/SDL2/CMakeLists.txt")
    add_subdirectory("ThirdParty/SDL2")
    message("Bluemetal: Found SDL2 as a recursive submodule.")
else()
    find_package(SDL2 CONFIG REQUIRED)
endif()


add_subdirectory("ThirdParty")
add_subdirectory("Source")


