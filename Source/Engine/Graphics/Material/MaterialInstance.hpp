#pragma once

#include "Material/Export.h"
#include "Material/Material.hpp"

class BLUEMETAL_MATERIAL_API blMaterialInstance {
public:
    blMaterialInstance(std::shared_ptr<blMaterial> material);
    ~blMaterialInstance();

private:
    vk::DescriptorSet _descriptor;
};
