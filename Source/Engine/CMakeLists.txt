set(Sources "")

list(APPEND Sources

  # Noodle
  # "Noodle/Noodle.cpp"
  # "Noodle/NoodleLexer.cpp"
  # "Noodle/NoodleToken.cpp"

  # Audio
  "Audio/AudioSystem.cpp"
  "Audio/Source.cpp"
  "Audio/Listener.cpp"
  "Audio/Sound.cpp"

  "Graphics/Window/Display.cpp"
  "Graphics/Window/Window.cpp"

  # Graphics
  "Graphics/volk.cpp"
  # "Graphics/RenderDoc.cpp"
  "Graphics/Instance.cpp"
  "Graphics/PhysicalDevice.cpp"
  # "Graphics/RenderWindow.cpp"
  "Graphics/Device.cpp"
  "Graphics/Resource/Buffer.cpp"
  "Graphics/Resource/Image.cpp"
  "Graphics/Resource/Shader.cpp"
  #"Graphics/ShaderReflection.cpp"
  "Graphics/Resource/DescriptorSetLayoutCache.cpp"
  #"Graphics/DescriptorSetAllocator.cpp"
  "Graphics/Resource/PipelineLayoutCache.cpp"
  #"Graphics/DescriptorAllocator.cpp"
  "Graphics/Resource/Pipeline.cpp"
  "Graphics/Window/Swapchain.cpp"
  "Core/FrameCounter.cpp"
  "Graphics/Mesh.cpp"
  "Graphics/Renderer/PresentPass.cpp"
  "Graphics/Renderer/Renderer.cpp"
  "Graphics/GraphicsSubsystem.cpp"
  # "Graphics/GeometryPass.cpp"

  "World/RenderActor.cpp"

  # imgui
  "imgui/imgui.cpp"
  "imgui/imgui_demo.cpp"
  "imgui/imgui_draw.cpp"
  "imgui/imgui_tables.cpp"
  "imgui/imgui_widgets.cpp"
  "imgui/imgui_impl_sdl2.cpp"
  "imgui/imgui_impl_vulkan.cpp"

  "Engine/SDLSubsystem.cpp"
  "Engine/AudioSubsystem.cpp"

  "Engine/ImGuiSubsystem.cpp"
  "Engine/Engine.cpp"
  )

add_library(Bluemetal STATIC ${Sources})

target_include_directories(Bluemetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")
target_link_libraries(Bluemetal ${Vulkan_LIBRARY} glm::glm-header-only spirv-reflect-static SDL2::SDL2 fmt::fmt FMOD)

# generate export header
include(GenerateExportHeader)

generate_export_header(
  Bluemetal
  BASE_NAME "BLUEMETAL"
  EXPORT_MACRO_NAME "BLUEMETAL_API"
  DEPRECATED_MACRO_NAME "BLUEMETAL_DEPRECATED"
  EXPORT_FILE_NAME "${CMAKE_CURRENT_SOURCE_DIR}/Export.h")