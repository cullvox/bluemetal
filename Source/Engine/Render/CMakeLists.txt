include(bluemetallib)

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

bl_add_library(BLRender Render "${RenderSources}")
target_include_directories(BLRender PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(BLRender spirv-reflect-static ${Vulkan_LIBRARIES})

target_link_libraries(BLRender bl::Core bl::Math bl::Window)
