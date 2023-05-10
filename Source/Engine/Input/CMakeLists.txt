include(BloodLustLibrary)

list(APPEND InputSources
    "Source/InputSystem.cpp")

bl_add_library(BloodLustInput Input "${InputSources}")

target_link_libraries(BloodLustInput bl::Core bl::Window)