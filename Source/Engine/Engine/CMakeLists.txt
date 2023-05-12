include(bluemetallib)

list(APPEND EngineSources
    "Source/Engine.cpp")

bl_add_library(BloodLustEngine Engine "${EngineSources}")

target_link_libraries(BloodLustEngine bl::Core bl::Noodle bl::Math bl::Window bl::Render bl::Input bl::ImGui)