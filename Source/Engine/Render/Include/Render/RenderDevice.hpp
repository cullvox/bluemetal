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

    vk::Instance getInstance() const noexcept;
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
    void immediateSubmit(std::function<void(vk::CommandBuffer)> recorder);
    void waitForDevice() const noexcept;

private:
    blRenderDevice& operator=(const blRenderDevice& copy) = delete;
    blRenderDevice& operator=(blRenderDevice&& move) = delete;

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

    vk::Instance        _instance;
    vk::PhysicalDevice  _physicalDevice;
    uint32_t            _graphicsFamilyIndex, _presentFamilyIndex;
    vk::Device          _device;
    vk::Queue           _graphicsQueue, _presentQueue;
    vk::CommandPool     _commandPool;
    VmaAllocator        _allocator;
};