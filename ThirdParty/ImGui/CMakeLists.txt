list(APPEND ImGuiSources
    "Source/imgui.cpp"
    "Source/imgui_demo.cpp"
    "Source/imgui_draw.cpp"
    "Source/imgui_tables.cpp"
    "Source/imgui_widgets.cpp")

add_library(ImGui STATIC ${ImGuiSources})
target_include_directories(ImGui PUBLIC "Include")
find_package(SDL2 CONFIG REQUIRED)

target_link_libraries(ImGui SDL2::SDL2)