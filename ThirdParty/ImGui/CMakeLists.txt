list(APPEND ImGuiSources
    "Source/imgui.cpp"
    "Source/imgui_demo.cpp"
    "Source/imgui_draw.cpp"
    "Source/imgui_tables.cpp"
    "Source/imgui_widgets.cpp"
    
    "Source/imgui_impl_sdl2.cpp"
    "Source/imgui_impl_vulkan.cpp")

add_library(ImGui SHARED ${ImGuiSources})
target_include_directories(ImGui PUBLIC "Include")
find_package(SDL2 CONFIG REQUIRED)

include(GenerateExportHeader)
generate_export_header(
    ImGui
    EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Include/${lib_short_name}/Export.h")

target_link_libraries(ImGui SDL2::SDL2)

find_package(Vulkan REQUIRED)

target_include_directories(ImGui PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(ImGui ${Vulkan_LIBRARIES})