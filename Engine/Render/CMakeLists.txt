
list(APPEND RenderSources
    "Source/RenderDevice.cpp"  
    "Source/RenderDevice.cpp"
    "Source/Buffer.cpp"
    "Source/Image.cpp"
    "Source/Shader.cpp"
    "Source/Pipeline.cpp"
    "Source/Swapchain.cpp"
    "Source/Renderer.cpp"
    "Source/FrameCounter.cpp")

add_library(BloodLustRender SHARED ${RenderSources})
add_library(bl::Render ALIAS BloodLustRender)
target_include_directories(BloodLustRender PUBLIC "Include")
target_link_libraries(BloodLustRender bl::Core bl::Math bl::Window)

find_package(Vulkan REQUIRED)

target_include_directories(BloodLustRender PUBLIC ${Vulkan_INCLUDE_DIRS})
target_link_libraries(BloodLustRender spirv-reflect-static ${Vulkan_LIBRARIES})

install(TARGETS BloodLustCore 
        EXPORT BloodLustCore
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})