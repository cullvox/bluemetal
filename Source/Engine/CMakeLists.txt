set(Sources "")

list(APPEND Sources
  # Math
  "Math/Vector2.cpp"
  "Math/Vector3.cpp"
  "Math/Vector4.cpp"

  # Noodle
  "Noodle/Noodle.cpp"
  "Noodle/NoodleLexer.cpp"
  "Noodle/NoodleToken.cpp"

  # Audio
  "Audio/AudioSystem.cpp"
  "Audio/AudioListener.cpp"
  "Audio/AudioSource.cpp"
  "Audio/Sound.cpp"

  # Graphics
  "Graphics/Window/Window.cpp"
  "Graphics/Window/Display.cpp"
  "Graphics/Instance.cpp"
  "Graphics/PhysicalDevice.cpp"
  "Graphics/Device.cpp"
  "Graphics/Buffer.cpp"
  "Graphics/Image.cpp"
  "Graphics/Shader.cpp"
  "Graphics/ShaderReflection.cpp"
  "Graphics/DescriptorLayoutCache.cpp"
  "Graphics/DescriptorAllocator.cpp"
  "Graphics/Pipeline.cpp"
  "Graphics/Swapchain.cpp"
  "Graphics/FrameCounter.cpp"
  # "Render/Mesh.cpp"
  "Graphics/PresentPass.cpp"
  "Graphics/Renderer.cpp"
  "Graphics/GeometryPass.cpp"

  # imgui
  "imgui/imgui.cpp"
  "imgui/imgui_demo.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/imgui_impl_sdl2.cpp"
  "imgui/imgui_impl_vulkan.cpp"
  "imgui/imgui_impl_bluemetal.cpp"

  "Engine/AudioSubsystem.cpp"
  "Engine/GraphicsSubsystem.cpp"
  "Engine/Engine.cpp"
  )

add_library(Bluemetal STATIC ${Sources})

target_include_directories(Bluemetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Bluemetal ${Vulkan_LIBRARY} spirv-reflect-static SDL2::SDL2 fmt::fmt FMOD)

# generate export header
include(GenerateExportHeader)

generate_export_header(
  Bluemetal
  BASE_NAME "BLUEMETAL"
  EXPORT_MACRO_NAME "BLUEMETAL_API"
  DEPRECATED_MACRO_NAME "BLUEMETAL_DEPRECATED"
  EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Export.h")