### Sets the minimal required CMake version to run this script.
cmake_minimum_required(VERSION 3.13)

### Sets project name and version.
project(cmake_discord_game_sdk VERSION 0.1)

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



add_library(discord_game_sdk SHARED IMPORTED GLOBAL)

### Installs Discord Game SDK binaries
if (OS STREQUAL "Windows")
    set_target_properties(discord_game_sdk PROPERTIES
        IMPORTED_LOCATION   "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.dll"
        IMPORTED_IMPLIB     "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.dll.lib")
elseif (OS STREQUAL "Linux")
    set_target_properties(discord_game_sdk PROPERTIES
        IMPORTED_LOCATION   "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.so")
elseif (OS STREQUAL "Macos")
    set_target_properties(discord_game_sdk PROPERTIES
        IMPORTED_LOCATION   "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.dylib")
endif ()

set_target_properties(discord_game_sdk PROPERTIES INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_LIST_DIR}/c")
