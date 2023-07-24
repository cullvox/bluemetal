#pragma once

#include "Device.h"

namespace bl
{

class BLUEMETAL_API Shader
{
public:
    Shader(std::shared_ptr<Device> device, const std::vector<uint32_t>& binary);
    ~Shader();

    VkShaderStageFlagBits getStage();
    VkShaderModule getModule();
    std::vector<uint32_t> getBinary();

private:
    void createShaderModule(const std::vector<uint32_t>& binary);

    std::shared_ptr<Device>         m_device;
    VkShaderModule                  m_module;
    std::vector<uint32_t>           m_binary;
};

} // namespace bl