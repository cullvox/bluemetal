cmake_minimum_required(VERSION 3.10)
project(DarkRed)

# Compile commands for clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if(CMAKE_EXPORT_COMPILE_COMMANDS)
  set(CMAKE_CXX_STANDARD_INCLUDE_DIRECTORIES 
      ${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES})
endif()

# Build some libraries
add_subdirectory("lib")

set(sources
    "src/core/Debug.cpp"
    "src/config/Config.cpp"
    "src/render/RenderDevice.cpp"
    "src/render/Buffer.cpp"
    "src/Main.cpp")

add_executable(DarkRed ${sources})
set_property(TARGET DarkRed PROPERTY CXX_STANDARD 20)
target_include_directories(DarkRed PUBLIC "src")

# Link
target_link_libraries(DarkRed fmt)

# Include vulkan headers
find_package(Vulkan REQUIRED)

target_include_directories(DarkRed PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(DarkRed ${Vulkan_LIBRARY})