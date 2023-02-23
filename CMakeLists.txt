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

    "src/config/ConfigToken.cpp"
    "src/config/ConfigLexer.cpp"
    "src/config/ConfigParser.cpp"
    "src/config/Config.cpp"

    "src/Window/GLFW.cpp"
    "src/Window/VideoMode.cpp"
    "src/Window/Screen.cpp"
    "src/Window/Window.cpp"
    
    "src/Render/RenderDevice.cpp"
    "src/Render/Swapchain.cpp"
    "src/Render/Buffer.cpp"
    "src/Main.cpp")

add_executable(DarkRed ${sources})
set_property(TARGET DarkRed PROPERTY CXX_STANDARD 20)
# Add -O0 to remove optimizations when using gcc
IF(CMAKE_COMPILER_IS_GNUCC)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
ENDIF(CMAKE_COMPILER_IS_GNUCC)

target_include_directories(DarkRed PUBLIC "src")

# Link
target_link_libraries(DarkRed spdlog glfw glm)

# Include vulkan headers
find_package(Vulkan REQUIRED)

target_include_directories(DarkRed PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(DarkRed ${Vulkan_LIBRARY})