#pragma once

#include "Material/Export.h"
#include "Material/Material.hpp"

class BLOODLUST_MATERIAL_API blMaterialInstance
{
public:
    blMaterialInstance(std::shared_ptr<blMaterial> material);
    ~blMaterialInstance();



private:
    vk::DescriptorSet _descriptor;
};
