#pragma once

#include "Precompiled.h"
#include "Export.h"

class BLUEMETAL_API blInstance
{
public:
    blInstance(std::optional<uint32_t> physicalDeviceIndex = std::nullopt);
    ~blInstance();

    VkInstance getInstance();
    VkPhysicalDevice getPhysicalDevice();
    std::string getDeviceName();
    std::string getVendorName();
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);

private:
    std::vector<const char*> getExtensionsForSDL();
    std::vector<const char*> getExtensions();
    std::vector<const char*> getValidationLayers();
    void createInstance();
    void choosePhysicalDevice(std::optional<uint32_t> physicalDeviceIndex);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,VkDebugUtilsMessageTypeFlagsEXT,const VkDebugUtilsMessengerCallbackDataEXT*,void*) noexcept;
    
    VkInstance                  _instance;
    VkDebugUtilsMessengerEXT    _messenger;
    VkPhysicalDevice            _physicalDevice;
};