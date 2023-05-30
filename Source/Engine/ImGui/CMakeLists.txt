include(bluemetallib)

list(APPEND ImGuiSources
    "Source/ImGui.cpp")

bl_add_library(BLImGui ImGui "${ImGuiSources}")
target_link_libraries(BLImGui ImGui bl::Core bl::Window bl::Input bl::Render)