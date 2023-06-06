include(bluemetallib)

list(APPEND WindowSources
    "Source/Display.cpp"
    "Source/Window.cpp")

bl_add_library(BLWindow Window "${WindowSources}")
target_link_libraries(BLWindow bl::Core bl::Math)

# Find required libraries
find_package(Vulkan REQUIRED)

option(SDL_VULKAN "" ON)

target_link_libraries(BLWindow ${Vulkan_LIBRARIES} SDL2::SDL2)
target_include_directories(BLWindow PUBLIC ${Vulkan_INCLUDE_DIRS})