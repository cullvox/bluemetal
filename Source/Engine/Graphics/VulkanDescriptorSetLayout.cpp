#include "VulkanDescriptorSetLayout.h"
#include "GraphicsSystem.h"
#include "VulkanDevice.h"

namespace bl {

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout()
    : _layout(VK_NULL_HANDLE)
    , _bindings({})
{
}

VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(std::span<VkDescriptorSetLayoutBinding> bindings)
{
    _bindings.assign(bindings.begin(), bindings.end());

    VkDescriptorSetLayoutCreateInfo info = {};
    info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    info.pNext = nullptr;
    info.flags = 0;
    info.bindingCount = _bindings.size();
    info.pBindings = _bindings.data();

    auto device = GraphicsSystem::Get()->GetDevice();
    VK_CHECK(vkCreateDescriptorSetLayout(device->Get(), &info, nullptr, &_layout));
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