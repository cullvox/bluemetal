#pragma once

#include "Render/Precompiled.hpp"

struct blBinding
{
    VkDescriptorType type;
};

struct blDescriptor
{
    uint32_t bindingsCount;
    std::vector<>
};

class blDescriptorManager
{
public:
    std::unordered_set<blDescriptor>

};

namespace std
{
    template<>
    struct hash<blDescriptor>
    {
        size_t operator()(const blDescriptor& descriptor)
        {

            

        }
    }
}