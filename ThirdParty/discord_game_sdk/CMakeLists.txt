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

### Final Target
set(CMAKE_DISCORD_GAME_SDK_TARGET_NAME ${PROJECT_NAME})
set(CMAKE_DISCORD_GAME_SDK_SOURCES
    "cpp/achievement_manager.cpp"
    "cpp/activity_manager.cpp"
    "cpp/application_manager.cpp"
    "cpp/core.cpp"
    "cpp/image_manager.cpp"
    "cpp/lobby_manager.cpp"
    "cpp/network_manager.cpp"
    "cpp/overlay_manager.cpp"
    "cpp/relationship_manager.cpp"
    "cpp/storage_manager.cpp"
    "cpp/store_manager.cpp"
    "cpp/types.cpp"
    "cpp/user_manager.cpp"
    "cpp/voice_manager.cpp")

add_library(discord_game_sdk STATIC ${CMAKE_DISCORD_GAME_SDK_SOURCES})

### Installs Discord Game SDK binaries
if (OS STREQUAL "Windows")
    target_link_libraries(discord_game_sdk "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.dll.lib")
elseif (OS STREQUAL "Linux")
    target_link_libraries(discord_game_sdk "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.so")
elseif (OS STREQUAL "Macos")
    target_link_libraries(discord_game_sdk "${CMAKE_CURRENT_LIST_DIR}/lib/x86_64/discord_game_sdk.dylib")
endif ()


target_include_directories(discord_game_sdk PUBLIC "cpp")
