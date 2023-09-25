#pragma once

#include "Graphics/Pipeline.hpp"
#include "Graphics/RenderDevice.hpp"
#include "Material/Export.h"
#include "Math/Vector4.hpp"
#include <vulkan/vulkan_handles.hpp>

enum class blMaterialPass { eOpaque,
    eTransparent };

class BLUEMETAL_MATERIAL_API blMaterial {
public:
    blMaterial(blRenderDevice& renderDevice, blPipeline& pipeline);
    ~blMaterial();

    void setScalarParameter(std::string_view name, float value);
    void setVectorParameter(std::string_view name, blVector4f value);

private:
    blRenderDevice& _renderDevice;
    blPipeline& _pipeline;
    vk::UniqueDescriptorSet _descriptor;
};