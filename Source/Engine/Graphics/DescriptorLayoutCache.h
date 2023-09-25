#pragma once

#include "Device.h"
#include "ShaderReflection.h"

namespace bl
{

struct BLUEMETAL_API DescriptorLayoutInfo
{
    uint32_t                                    set;
    std::vector<VkDescriptorSetLayoutBinding>   bindings;

    bool operator==(const DescriptorLayoutInfo& other) const;

    size_t hash() const;
};

class DescriptorLayoutCache
{
public:
    DescriptorLayoutCache(GraphicsDevice* pDevice);
    ~DescriptorLayoutCache();

    VkDescriptorSetLayout createLayout(const VkDescriptorSetLayoutCreateInfo& createInfo);

private:
    struct DescriptorLayoutHash
    {
        size_t operator()(const DescriptorLayoutInfo& value) const
        {
            return value.hash();
        }
    };

    GraphicsDevice*             m_pDevice;
    std::unordered_map<
        DescriptorLayoutInfo, 
        VkDescriptorSetLayout, 
        DescriptorLayoutHash>   m_cache;
};

} // namespace bl
