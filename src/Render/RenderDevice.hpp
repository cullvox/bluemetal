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
    RenderDevice(Window& initialWindow);
    ~RenderDevice();

    VkInstance          GetInstance() const noexcept;
    VkPhysicalDevice    GetPhysicalDevice() const noexcept;
    uint32_t            GetGraphicsFamilyIndex() const noexcept;
    uint32_t            GetPresentFamilyIndex() const noexcept;
    VkDevice            GetDevice() const noexcept;
    VkQueue             GetGraphicsQueue() const noexcept;
    VkQueue             GetPresentQueue() const noexcept;
    VmaAllocator        GetAllocator() const noexcept;
    inline bool         AreQueuesSame() const noexcept { return m_graphicsFamilyIndex == m_presentFamilyIndex; }
private:
    std::vector<const char*> GetValidationLayers() const;
    std::vector<const char*> GetInstanceExtensions() const;
    void                CreateInstance();
    void                ChoosePhysicalDevice();
    std::vector<const char*> GetDeviceExtensions() const;
    void                CreateDevice();
    void                CreateAllocator();
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,const VkDebugUtilsMessengerCallbackDataEXT*,void*);
    
    Window&             m_window;
    VkInstance          m_instance;
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice            m_device;
    VkQueue             m_graphicsQueue, m_presentQueue;
    VmaAllocator        m_allocator;
};

} /* namespace bl */

inline RenderDevice g_renderDevice{g_window};