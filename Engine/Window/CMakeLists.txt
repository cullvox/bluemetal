list(APPEND WindowSources
    "Source/Display.cpp"
    "Source/Window.cpp")

add_library(BloodLustWindow SHARED ${WindowSources})
add_library(bl::Window ALIAS BloodLustWindow)
target_include_directories(BloodLustWindow PUBLIC "Include")
target_link_libraries(BloodLustWindow bl::Core bl::Math)

# Find required libraries
find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(BloodLustWindow SDL2::SDL2)