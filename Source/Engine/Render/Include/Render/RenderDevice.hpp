#pragma once

#include "Core/Precompiled.hpp"
#include "Window/Window.hpp"
#include "Render/Precompiled.hpp"
#include "Render/Export.h"

class BLOODLUST_RENDER_API blRenderDevice
{
public:
    blRenderDevice(std::shared_ptr<blWindow> window);
    ~blRenderDevice() noexcept;

    std::shared_ptr<blWindow> getWindow() const noexcept;
    vk::Instance getInstance() const noexcept;
    vk::SurfaceKHR getSurface() const noexcept;
    vk::PhysicalDevice getPhysicalDevice() const noexcept;
    uint32_t getGraphicsFamilyIndex() const noexcept;
    uint32_t getPresentFamilyIndex() const noexcept;
    vk::Device getDevice() const noexcept;
    vk::Queue getGraphicsQueue() const noexcept;
    vk::Queue getPresentQueue() const noexcept;
    vk::CommandPool getCommandPool() const noexcept;
    VmaAllocator getAllocator() const noexcept;
    bool areQueuesSame() const noexcept;
    vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, 
        vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
    void immediateSubmit(const std::function<void(vk::CommandBuffer)>& recorder) const;
    void waitForDevice() const noexcept;
    const char* getVendorName() const noexcept;
    const char* getDeviceName() const noexcept; 

private:
    std::vector<const char*> getValidationLayers() const;
    std::vector<const char*> getInstanceExtensions() const;
    std::vector<const char*> getDeviceExtensions() const;
    void createInstance();
    void choosePhysicalDevice();
    void createDevice();
    void createCommandPool();
    void createAllocator();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT*,void*) noexcept;

    std::shared_ptr<blWindow> _window;

    vk::UniqueInstance      _instance;
    vk::UniqueSurfaceKHR    _surface;
    vk::PhysicalDevice      _physicalDevice;
    uint32_t                _graphicsFamilyIndex, _presentFamilyIndex;
    vk::UniqueDevice        _device;
    vk::Queue               _graphicsQueue, _presentQueue;
    vk::UniqueCommandPool   _commandPool;

    VmaAllocator            _allocator;
};