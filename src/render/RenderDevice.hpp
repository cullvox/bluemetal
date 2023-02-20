#include <memory>

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#include "config/Config.hpp"
#include "Window.hpp"
class CRenderDevice
{
public:
    CRenderDevice(IWindow* pWindow, CConfig* pConfig = g_pConfig.get());
    ~CRenderDevice();

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
    CConfig*            m_pConfig;
    IWindow*            m_pWindow;
    VkInstance          m_instance;
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice            m_device;
    VkQueue             m_graphicsQueue, m_presentQueue;
    VmaAllocator        m_allocator;
};

inline std::unique_ptr<CRenderDevice> g_pRenderDevice;