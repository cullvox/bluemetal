#pragma once

#include "Render/Instance.h"
#include "Window/Window.h"

namespace bl
{

class BLUEMETAL_API Device
{
public:
    Device(std::shared_ptr<Instance> instance, std::shared_ptr<PhysicalDevice> physicalDevice, std::shared_ptr<Window> temporaryWindow);
    ~Device();

    std::shared_ptr<PhysicalDevice> getPhysicalDevice();

    VkDevice getHandle();
    uint32_t getGraphicsFamilyIndex();
    uint32_t getPresentFamilyIndex();
    VkQueue getGraphicsQueue();
    VkQueue getPresentQueue();
    VkCommandPool getCommandPool();
    VmaAllocator getAllocator();
    bool areQueuesSame();
    std::string getVendorName();
    std::string getDeviceName();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    void immediateSubmit(const std::function<void(VkCommandBuffer)>& recorder);
    void waitForDevice();
    
private:
    std::vector<const char*> getValidationLayers();
    std::vector<const char*> getExtensions();
    void createDevice(std::shared_ptr<Window> window);
    void createCommandPool();
    void createAllocator();

    std::shared_ptr<Instance>       m_instance;
    std::shared_ptr<PhysicalDevice> m_physicalDevice;
    uint32_t                        m_graphicsFamilyIndex, m_presentFamilyIndex;
    VkDevice                        m_device;
    VkQueue                         m_graphicsQueue, m_presentQueue;
    VkCommandPool                   m_commandPool;
    VmaAllocator                    m_allocator;
};

} // namespace bl