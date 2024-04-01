#pragma once

#include "Export.h"
#include "Graphics/Pipeline.h"
#include "Graphics/RenderDevice.h"
#include "Math/Vector4.h"

enum class MaterialPass { 
    eOpaque,
    eTransparent
};

class BLUEMETAL_API GfxMaterial 
{
public:

    struct CreateInfo 
    {
        GfxPipeline& pipeline;
        MaterialPass pass;
    };

    GfxMaterial(blRenderDevice& renderDevice, blPipeline& pipeline);
    ~GfxMaterial();

    void setScalarParameter(std::string_view name, float value);
    void setVectorParameter(std::string_view name, blVector4f value);

private:
    blRenderDevice& _renderDevice;
    blPipeline& _pipeline;
    vk::UniqueDescriptorSet _descriptor;
};