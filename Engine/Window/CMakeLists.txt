list(APPEND WindowSources
    "Source/GLFW.cpp"
    "Source/VideoMode.cpp"
    "Source/Screen.cpp"
    "Source/Window.cpp")

add_library(BloodLustWindow SHARED ${WindowSources})
add_library(bl::Window ALIAS BloodLustWindow)
target_include_directories(BloodLustWindow PUBLIC "Include")
target_link_libraries(BloodLustWindow bl::Core bl::Math)

# Find required libraries
pkg_check_modules(GLFW REQUIRED IMPORTED_TARGET glfw3)

target_include_directories(BloodLustWindow PRIVATE ${GLFW_INCLUDE_DIR})
target_link_libraries(BloodLustWindow ${GLFW_LIBRARIES})