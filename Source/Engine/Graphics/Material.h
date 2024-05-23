#pragma once

#include "Resource/Resource.h"
#include "DescriptorSet.h"

namespace bl
{

class Material : public Resource
{
public:
    Material(const nlohmann::json& data);
    ~Material();

    virtual void Load();
    virtual void Unload();

private:
    std::unique_ptr<Pipeline> pipeline;
    std::vector<VkDescriptorSet> _descriptors;
};

} // namespace bl