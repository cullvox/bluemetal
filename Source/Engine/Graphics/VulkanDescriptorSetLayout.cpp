#include "VulkanDescriptorSetLayout.h"
#include "Graphics/VulkanDescriptorSetLayoutCache.h"
#include "GraphicsSystem.h"
#include "VulkanDevice.h"
#include "Vulkan.h"

namespace bl {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
    : _layout(VK_NULL_HANDLE)
    , _bindings({})
{
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(VulkanDescriptorSetLayoutCache* cache, std::span<VkDescriptorSetLayoutBinding> bindings)
    : _cache(cache)
{
    _bindings.assign(bindings.begin(), bindings.end());
    _layout = cache->AcquireRaw(bindings);
}

VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
{
}

VkDescriptorSetLayout VulkanDescriptorSetLayout::GetLayout() const
{
    return _layout;
}

std::span<const VkDescriptorSetLayoutBinding> VulkanDescriptorSetLayout::GetBindings() const
{
    return std::span{_bindings};
}


} // namespace bl