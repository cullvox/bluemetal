list(APPEND WindowSources
    "Source/Display.cpp"
    "Source/Window.cpp")

add_library(BloodLustWindow SHARED ${WindowSources})
add_library(bl::Window ALIAS BloodLustWindow)
target_include_directories(BloodLustWindow PUBLIC "Include")
target_link_libraries(BloodLustWindow bl::Core bl::Math)

# Find required libraries
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2)

target_include_directories(BloodLustWindow PRIVATE ${SDL2_INCLUDE_DIR})
target_link_libraries(BloodLustWindow ${SDL2_LIBRARIES})