#pragma once

#include "Resource/Resource.h"
#include "DescriptorSet.h"

namespace bl
{

class Material : public Resource
{
public:
    Material(Pipeline* pipeline);
    ~Material();

private:
    Pipeline* pipeline;
    std::vector<Descriptor> _descriptors;
    std::vector<Uniform> _uniforms;
};

} // namespace bl