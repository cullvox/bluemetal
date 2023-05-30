include(bluemetallib)

list(APPEND InputSources
    "Source/InputSystem.cpp")

bl_add_library(BLInput Input "${InputSources}")

target_link_libraries(BLInput bl::Core bl::Window)