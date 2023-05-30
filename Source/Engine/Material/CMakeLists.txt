include(bluemetallib)

list(APPEND MaterialSources
    "Source/Material.cpp"
    "Source/MaterialInstance.cpp")

bl_add_library(BLMaterial Material "${MaterialSources}")
target_include_directories(BLMaterial PUBLIC "Include")
target_link_libraries(BLMaterial bl::Core bl::Render)