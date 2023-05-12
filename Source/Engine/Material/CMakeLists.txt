include(bluemetallib)

list(APPEND MaterialSources
    "Source/Material.cpp"
    "Source/MaterialInstance.cpp")

bl_add_library(BloodLustMaterial Material "${MaterialSources}")
target_include_directories(BloodLustMaterial PUBLIC "Include")
target_link_libraries(BloodLustMaterial bl::Core bl::Render)