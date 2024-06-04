#pragma once

#include "Precompiled.h"
#include "Material.h"
#include "Buffer.h"

namespace bl
{

/** @brief */
class MaterialInstance : public MaterialBase
{
public:
    MaterialInstance(Device* _device, Material* source);
    ~MaterialInstance();

private:
    Device* _device;
    Material* _material;
    std::unordered_map<std::string, Buffer> _uniformBuffers;
};

} // namespace bl