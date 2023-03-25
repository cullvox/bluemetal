list(APPEND ImGuiSources
    "Source/imgui.cpp"
    "Source/imgui_demo.cpp"
    "Source/imgui_draw.cpp"
    "Source/imgui_tables.cpp"
    "Source/imgui_widgets.cpp"
    "Source/imgui_impl_sdl2.cpp"
    "Source/imgui_impl_vulkan.cpp"
    "Source/imgui_impl_bloodlust.cpp")

add_library(BloodLustImGui STATIC ${ImGuiSources})
add_library(bl::ImGui ALIAS BloodLustImGui)
target_include_directories(BloodLustImGui PUBLIC "Include")
target_link_libraries(BloodLustImGui bl::Window bl::Input bl::Render)