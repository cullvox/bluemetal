### Sets the minimal required CMake version to run this script.
cmake_minimum_required(VERSION 3.13)

### Sets project name and version.
project(discord_social_sdk VERSION 0.1)

### OS Requirements
### - OS must contain an identifier corresponding to your current operating system: Windows, Macos, Linux.
if (NOT OS) # If OS is not defined, it tries to find it.
    if (WIN32)
        set(OS Windows)
    elseif (APPLE)
        set(OS Macos)
    elseif (UNIX)
        set(OS Linux)
    else ()
        message(FATAL_ERROR "Cannot detect operating system")
    endif ()
    message(STATUS "OS was not defined, automatically set to ${OS}")
elseif(NOT OS STREQUAL "Windows" AND NOT OS STREQUAL "Macos" AND NOT OS STREQUAL "Linux") # OS can only have following values: Windows, Macos, Linux.
    message(FATAL_ERROR "OS has wrong value. Was: ${OS}, must be: Windows|Macos|Linux")
endif ()

add_library(discord_social_sdk SHARED IMPORTED GLOBAL)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set (BUILD_TYPE "release")
else ()
    set (BUILD_TYPE "debug")
endif ()

set (ROOT_PATH "${CMAKE_CURRENT_LIST_DIR}/lib/${BUILD_TYPE}")
set (LIB_PATH ${ROOT_PATH})

if (CMAKE_SYSTEM_PROCESSOR STREQUAL "arm64")
    set (LIB_PATH "${ROOT_PATH}/arm64")
endif ()

### Installs Discord Game SDK binaries
if (OS STREQUAL "Windows")
    set_target_properties(discord_social_sdk PROPERTIES
        IMPORTED_LOCATION   "${ROOT_PATH}/discord_partner_sdk.dll"
        IMPORTED_IMPLIB     "${LIB_PATH}/discord_partner_sdk.lib")
elseif (OS STREQUAL "Linux")
    set_target_properties(discord_social_sdk PROPERTIES
        IMPORTED_LOCATION   "${ROOT_PATH}/libdiscord_partner_sdk.so")
elseif (OS STREQUAL "Macos")
    set_target_properties(discord_social_sdk PROPERTIES
        IMPORTED_LOCATION   "${ROOT_PATH}/libdiscord_partner_sdk.dylib")
endif ()

set_target_properties(discord_social_sdk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/include")
