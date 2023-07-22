set(Sources "")

list(APPEND Sources
  # Math
  "Math/Vector2.cpp"
  "Math/Vector3.cpp"
  "Math/Vector4.cpp"

  # Noodle
  

  # Window
  "Window/Window.cpp"
  "Window/Display.cpp"

  # Render
  "Render/Instance.cpp"
  "Render/Device.cpp"
  "Render/Buffer.cpp"
  "Render/Image.cpp"
  "Render/Shader.cpp"
  "Render/DescriptorLayoutCache.cpp"
  "Render/DescriptorAllocator.cpp"
  "Render/Pipeline.cpp"
  "Render/Swapchain.cpp"
  "Render/Renderer.cpp"
  "Render/FrameCounter.cpp"
  "Render/Mesh.cpp"
  "Render/PresentRenderPass.cpp")

add_library(BlueMetal STATIC ${Sources})

target_include_directories(BlueMetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(BlueMetal ${Vulkan_LIBRARY} spirv-reflect-static SDL2::SDL2 fmt)

# generate export header
include(GenerateExportHeader)

generate_export_header(
  BlueMetal
  BASE_NAME "BLUEMETAL"
  EXPORT_MACRO_NAME "BLUEMETAL_API"
  DEPRECATED_MACRO_NAME "BLUEMETAL_DEPRECATED"
  EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Export.h")