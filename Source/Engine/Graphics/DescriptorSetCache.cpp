#include "Device.h"
#include "DescriptorSetCache.h"

namespace bl
{

DescriptorSetCache::DescriptorSetCache(Device* device, uint32_t maxSets, std::span<DescriptorRatio> ratios)
    : _device(device)
{
    for (auto ratio : ratios)
        _ratios.push_back(ratio);

    VkDescriptorPool pool = CreatePool(maxSets);
    _setsPerPool = (uint32_t)(maxSets * 1.5);

    _freePools.push_back(pool);
}

DescriptorSetCache::~DescriptorSetCache()
{
    for (auto pool : _freePools)
        vkDestroyDescriptorPool(_device->Get(), pool, nullptr);
    for (auto pool : _usedPools)
        vkDestroyDescriptorPool(_device->Get(), pool, nullptr);
}

VkDescriptorSet DescriptorSetCache::Allocate(VkDescriptorSetLayout layout)
{

    // Check if there is already a free set from the same layout.
    auto it = _freeSets.find(layout);
    if (it != _freeSets.end())
    {
        auto& freeSets = (*it).second;
        
        if (freeSets.size() > 0)
        {
            
            // Find the first descriptor set and then remove it from the set.
            auto first = freeSets.begin();
            auto set = (*first);

            freeSets.erase(set);

            // The descriptor set is no longer considered free, return it.
            return set; 
        }
    }

    // Grab ourselves a useful pool and allocate a new set.
    VkDescriptorPool pool = GrabPool();

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.pNext = nullptr;
    allocInfo.descriptorPool = pool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &layout;

    VkDescriptorSet set;
    VkResult result = vkAllocateDescriptorSets(_device->Get(), &allocInfo, &set);

    if (result == VK_ERROR_OUT_OF_POOL_MEMORY || result == VK_ERROR_FRAGMENTED_POOL)
    {
        // Uh oh. Our pool wasn't large enough.
        // Allocate again with another pool.
        _usedPools.push_back(pool);

        pool = GrabPool();
        allocInfo.descriptorPool = pool;

        // If allocating again fails let someone else handle exception,
        // we can't do anything from here then.
        VK_CHECK(vkAllocateDescriptorSets(_device->Get(), &allocInfo, &set))
    }

    _freePools.push_back(pool);

    return set;
}

void DescriptorSetCache::Free(VkDescriptorSetLayout layout, VkDescriptorSet set)
{
    _freeSets[layout].insert(set);
}

VkDescriptorPool DescriptorSetCache::GrabPool()
{
    VkDescriptorPool pool;

    if (_freePools.size() != 0)
    {
        pool = _freePools.back();
        _freePools.pop_back();
        return pool;
    }
    
    pool = CreatePool(_setsPerPool);

    // Increment the amount of sets per pool up to a limit.
    _setsPerPool = uint32_t(_setsPerPool * 1.5);
    if (_setsPerPool > 4092)
    {
        _setsPerPool = 4092;
    }
    
    return pool;
}

VkDescriptorPool DescriptorSetCache::CreatePool(uint32_t setCount)
{
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const auto& ratio : _ratios)
    {
        poolSizes.emplace_back(ratio.type, (uint32_t)(ratio.ratio * setCount));
    }

    VkDescriptorPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    createInfo.flags = 0;
    createInfo.maxSets = setCount;
    createInfo.poolSizeCount = (uint32_t)poolSizes.size();
    createInfo.pPoolSizes = poolSizes.data();

    VkDescriptorPool pool = VK_NULL_HANDLE;
    VK_CHECK(vkCreateDescriptorPool(_device->Get(), &createInfo, nullptr, &pool))

    return pool;
}

void DescriptorSetCache::ResetPools()
{
    for (auto pool : _freePools)
        VK_CHECK(vkResetDescriptorPool(_device->Get(), pool, 0))
    for (auto pool : _usedPools)
        VK_CHECK(vkResetDescriptorPool(_device->Get(), pool, 0))

    _freePools.clear();
    _usedPools.clear();
}

} // namespace bl