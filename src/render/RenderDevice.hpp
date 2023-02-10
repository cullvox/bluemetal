#include <memory>

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#include "config/Config.hpp"

class CRenderDevice
{
public:
    CRenderDevice(CConfig* pConfig = g_pConfig.get());
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
    VkInstance          m_instance;
    VkPhysicalDevice    m_physicalDevice;
    uint32_t            m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice            m_device;
    VkQueue             m_graphicsQueue, m_presentQueue;
    VmaAllocator        m_allocator;
};

inline std::unique_ptr<CRenderDevice> g_pRenderDevice;