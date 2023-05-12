include(bluemetallib)

list(APPEND WindowSources
    "Source/Display.cpp"
    "Source/Window.cpp")

bl_add_library(BloodLustWindow Window "${WindowSources}")
target_link_libraries(BloodLustWindow bl::Core bl::Math)

# Find required libraries
find_package(Vulkan REQUIRED)
find_package(SDL2 CONFIG REQUIRED)

option(SDL_VULKAN "" ON)

target_link_libraries(BloodLustWindow ${Vulkan_LIBRARIES} SDL2::SDL2)
target_include_directories(BloodLustWindow PUBLIC ${Vulkan_INCLUDE_DIRS})