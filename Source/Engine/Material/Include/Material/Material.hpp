#pragma once

#include "Material/Export.h"
#include "Render/RenderDevice.hpp"
#include "Render/Pipeline.hpp"

enum class blMaterialType
{
    eOpaque
};

class BLOODLUST_MATERIAL_API blMaterial
{
public:
    blMaterial(
        std::shared_ptr<blRenderDevice> renderDevice);
    ~blMaterial();

private:
    vk::DescriptorSet _descriptor;
    std::shared_ptr<blPipeline> _pipeline;
};