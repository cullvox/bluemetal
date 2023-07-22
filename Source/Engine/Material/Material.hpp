#pragma once


#include "Math/Vector4.hpp"
#include "Render/RenderDevice.hpp"
#include "Render/Pipeline.hpp"
#include "Material/Export.h"
#include <vulkan/vulkan_handles.hpp>

enum class blMaterialPass
{
    eOpaque,
    eTransparent
};

class BLUEMETAL_MATERIAL_API blMaterial
{
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