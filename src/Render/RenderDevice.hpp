#pragma once

#include "Window/Window.hpp"

#include "vk_mem_alloc.h"
#include <vulkan/vulkan.h>

#include <memory>

namespace bl
{

class RenderDevice
{
public:
    RenderDevice(Window& window);
    ~RenderDevice();

    VkInstance          getInstance() const noexcept;
    VkPhysicalDevice    getPhysicalDevice() const noexcept;
    uint32_t            getGraphicsFamilyIndex() const noexcept;
    uint32_t            getPresentFamilyIndex() const noexcept;
    VkDevice            getDevice() const noexcept;
    VkQueue             getGraphicsQueue() const noexcept;
    VkQueue             getPresentQueue() const noexcept;
    VmaAllocator        getAllocator() const noexcept;
    inline bool         areQueuesSame() const noexcept { return m_graphicsFamilyIndex == m_presentFamilyIndex; }
private:
    std::vector<const char*> getValidationLayers() const;
    std::vector<const char*> getInstanceExtensions() const;
    void                createInstance();
    void                choosePhysicalDevice();
    std::vector<const char*> getDeviceExtensions() const;
    void                createDevice();
    void                createAllocator();
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,const VkDebugUtilsMessengerCallbackDataEXT*,void*);
    
    Window&             m_window;
    VkInstance          m_instance;
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice            m_device;
    VkQueue             m_graphicsQueue, m_presentQueue;
    VmaAllocator        m_allocator;
};

} /* namespace bl */