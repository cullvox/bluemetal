set(Sources "")

list(APPEND Sources

    # Math
    "Math/Vector2.cpp"
    "Math/Vector3.cpp"
    "Math/Vector4.cpp"

    # Render
    "Render/RenderDevice.cpp"
    "Render/Buffer.cpp"
    "Render/Image.cpp"
    "Render/Shader.cpp"
    "Render/Pipeline.cpp"
    "Render/Swapchain.cpp"
    "Render/Renderer.cpp"
    "Render/FrameCounter.cpp"
    "Render/Mesh.cpp"
    "Render/PresentRenderPass.cpp")

add_library(BlueMetal STATIC ${Sources})