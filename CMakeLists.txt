cmake_minimum_required(VERSION 3.10)
project(DarkRed)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

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

    "src/Math/Vector2.cpp"

    "src/config/ConfigToken.cpp"
    "src/config/ConfigLexer.cpp"
    "src/config/ConfigParser.cpp"
    "src/config/Config.cpp"

    "src/Window/GLFW.cpp"
    "src/Window/VideoMode.cpp"
    "src/Window/Screen.cpp"
    "src/Window/Window.cpp"
    
    "src/Render/RenderDevice.cpp"
    "src/Render/Image.cpp"
    "src/Render/Swapchain.cpp"
    "src/Render/Renderer.cpp"
    "src/Render/Buffer.cpp"
    "src/Main.cpp")

add_executable(DarkRed ${sources})
set_property(TARGET DarkRed PROPERTY CXX_STANDARD 20)

target_include_directories(DarkRed PUBLIC "src")

#target_compile_definitions(DarkRed PUBLIC SPDLOG_FMT_EXTERNAL) # Use an external version of FMT

find_package(spdlog REQUIRED)

# Link
target_link_libraries(DarkRed spdlog::spdlog glfw)

# Include vulkan headers
find_package(Vulkan REQUIRED)

target_include_directories(DarkRed PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(DarkRed ${Vulkan_LIBRARY})