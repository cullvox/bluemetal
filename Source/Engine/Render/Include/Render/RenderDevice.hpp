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

    VkInstance getInstance() const noexcept;
    VkPhysicalDevice getPhysicalDevice() const noexcept;
    uint32_t getGraphicsFamilyIndex() const noexcept;
    uint32_t getPresentFamilyIndex() const noexcept;
    VkDevice getDevice() const noexcept;
    VkQueue getGraphicsQueue() const noexcept;
    VkQueue getPresentQueue() const noexcept;
    VkCommandPool getCommandPool() const noexcept;
    VmaAllocator getAllocator() const noexcept;
    bool areQueuesSame() const noexcept;
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, 
        VkImageTiling tiling, VkFormatFeatureFlags features) const;
    void immediateSubmit(std::function<void(VkCommandBuffer)> recorder);
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

    VkInstance          _instance;
    VkPhysicalDevice    _physicalDevice;
    uint32_t            _graphicsFamilyIndex, _presentFamilyIndex;
    VkDevice            _device;
    VkQueue             _graphicsQueue, _presentQueue;
    VkCommandPool       _commandPool;
    VmaAllocator        _allocator;
};