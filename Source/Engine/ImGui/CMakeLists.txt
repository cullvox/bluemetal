include(bluemetallib)

list(APPEND ImGuiSources
    "Source/ImGui.cpp")

bl_add_library(BloodLustImGui ImGui "${ImGuiSources}")
target_link_libraries(BloodLustImGui ImGui bl::Core bl::Window bl::Input bl::Render)