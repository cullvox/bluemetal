list(APPEND InputSources
    "Source/InputSystem.cpp")

add_library(BloodLustInput SHARED ${InputSources})
add_library(bl::Input ALIAS BloodLustInput)
target_include_directories(BloodLustInput PUBLIC "Include")
target_link_libraries(BloodLustInput bl::Core bl::Window)