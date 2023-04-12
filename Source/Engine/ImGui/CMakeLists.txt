include(BloodLustLibrary)

list(APPEND ImGuiSources
    "Source/imgui_impl_sdl2.cpp"
    "Source/imgui_impl_vulkan.cpp"
    "Source/ImGui.cpp")

bl_add_library(BloodLustImGui ImGui ${ImGuiSources})
target_link_libraries(BloodLustImGui ImGui bl::Core bl::Window bl::Input bl::Render)