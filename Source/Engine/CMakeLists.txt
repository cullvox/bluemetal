set(Sources "")

list(APPEND Sources

  # Core
  "Core/FrameCounter.cpp"
  
  # Audio
  "Audio/AudioSystem.cpp"
  "Audio/Source.cpp"
  "Audio/Listener.cpp"
  "Audio/Sound.cpp"

  "Graphics/Display.cpp"
  #"Graphics/Window.cpp"
  "Graphics/VulkanInstance.cpp"
  "Graphics/VulkanPhysicalDevice.cpp"
  "Graphics/VulkanDevice.cpp"
  "Graphics/VulkanBuffer.cpp"
  "Graphics/VulkanImage.cpp"
  "Graphics/VulkanSampler.cpp"
  "Graphics/VulkanShader.cpp"
  "Graphics/VulkanBlockReflection.cpp"
  "Graphics/VulkanDescriptorSetReflection.cpp"
  "Graphics/VulkanDescriptorSetLayoutCache.cpp"
  "Graphics/VulkanPushConstantReflection.cpp"
  "Graphics/VulkanPipelineLayoutCache.cpp"
  "Graphics/VulkanPipeline.cpp"
  "Graphics/VulkanDescriptorSetAllocatorCache.cpp"
  "Graphics/VulkanSwapchain.cpp"
  "Graphics/Material.cpp"
  "Graphics/Mesh.cpp"
  "Graphics/Renderer.cpp"
  "Graphics/Texture.cpp"
  "Graphics/Texture2D.cpp"
  "Graphics/GraphicsSystem.cpp"

  # ImGui
  "ImGui/imgui.cpp"
  "ImGui/imgui_demo.cpp"
  "ImGui/imgui_draw.cpp"
  "ImGui/imgui_tables.cpp"
  "ImGui/imgui_widgets.cpp"
  "ImGui/imgui_impl_sdl2.cpp"
  "ImGui/imgui_impl_vulkan.cpp"
  "ImGui/ImGuiSystem.cpp"

  "Resource/Resource.cpp"
  "Resource/ResourceManager.cpp"

  "Engine/SDLInitializer.cpp"
  
  "Engine/Engine.cpp"
  )

add_library(Bluemetal STATIC ${Sources})

set_property(TARGET Bluemetal PROPERTY CXX_STANDARD 20)


target_include_directories(Bluemetal PUBLIC ${Vulkan_INCLUDE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}")

target_link_libraries(Bluemetal PUBLIC 
  ${Vulkan_LIBRARY} 
  glm::glm
  spirv-reflect-static 
  SDL3::SDL3 
  fmt::fmt
  FMOD
  spng_static
  volk
  GPUOpen::VulkanMemoryAllocator
  qoixx::qoixx)

target_precompile_headers(Bluemetal PUBLIC "Precompiled.h")
