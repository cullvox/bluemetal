#include "PipelineLayoutCache.h"

namespace bl 
{

PipelineLayoutCache::PipelineLayoutCache(std::shared_ptr<GfxDevice> device)
    : _device(device)
{
}

PipelineLayoutCache::~PipelineLayoutCache()
{
    for (auto& pair : _cache)
        vkDestroyPipelineLayout(_device->get(), pair.second, nullptr);
}

VkPipelineLayout PipelineLayoutCache::acquire(
    std::vector<VkDescriptorSetLayout>  setLayouts, 
    std::vector<VkPushConstantRange>    ranges)
{
    // Sort each value to ensure stability when hashing.
    std::sort(setLayouts.begin(), setLayouts.end());
    std::sort(ranges.begin(), ranges.end(), 
        [](VkPushConstantRange& a, VkPushConstantRange& b){
            return a.offset < b.offset;
        });

    PipelineLayoutInfo layoutInfo{setLayouts, ranges};

    // Check if the layout is in our cache.
    auto it = _cache.find(layoutInfo);

    // Return a found layout.
    if (it != _cache.end())
        return (*it).second;

    // Create a new pipeline instead.
    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0;
    createInfo.setLayoutCount = (uint32_t)setLayouts.size();
    createInfo.pSetLayouts = setLayouts.data();
    createInfo.pushConstantRangeCount = (uint32_t)ranges.size();
    createInfo.pPushConstantRanges = ranges.data();

    VkPipelineLayout layout = VK_NULL_HANDLE;
    VK_CHECK(vkCreatePipelineLayout(_device->get(), &createInfo, nullptr, &layout))

    // Add the new layout to the cache.
    _cache[layoutInfo] = layout;
    return layout;
}

} // namespace bl