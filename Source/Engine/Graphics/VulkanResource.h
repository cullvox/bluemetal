#pragma once

#include "Core/ReferenceCounted.h"

namespace bl
{

class VulkanResource : public ReferenceCounted
{
public:
    virtual ~VulkanResource() = default;
};

}