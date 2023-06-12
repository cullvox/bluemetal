#pragma once

#include "Material/Export.h"
#include "Render/RenderDevice.hpp"
#include "Render/Pipeline.hpp"
#include <vulkan/vulkan_handles.hpp>

enum class blMaterialPass
{
    eOpaque,
    eTransparent
};

class BLUEMETAL_MATERIAL_API blMaterial
{
public:
    blMaterial(std::shared_ptr<blRenderDevice> renderDevice, std::shared_ptr<blPipeline> pipeline);
    ~blMaterial();

private:
    vk::UniqueDescriptorSet _descriptor;
    std::shared_ptr<blPipeline> _pipeline;
};