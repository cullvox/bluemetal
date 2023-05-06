include(BloodLustLibrary)

list(APPEND RenderSources
    "Source/RenderDevice.cpp"
    "Source/Buffer.cpp"
    "Source/Image.cpp"
    "Source/Shader.cpp"
    "Source/Pipeline.cpp"
    "Source/Swapchain.cpp"
    "Source/Renderer.cpp"
    "Source/FrameCounter.cpp"

    "Source/Renderer/PresentRenderPass.cpp")

bl_add_library(BloodLustRender Render "${RenderSources}")
target_link_libraries(BloodLustRender bl::Core bl::Math BloodLustWindow)
target_precompile_headers(BloodLustRender PUBLIC "Include/Render/Precompiled.hpp")

find_package(Vulkan REQUIRED)

target_include_directories(BloodLustRender PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(BloodLustRender spirv-reflect-static ${Vulkan_LIBRARIES})