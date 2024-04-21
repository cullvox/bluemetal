#pragma once

#include "Graphics/GraphicsLimits.h"
#include "Graphics/Buffer.h"
#include "Actor.h"

namespace bl
{

class RenderActor : public Actor
{
public:
    RenderActor(
        const std::string& name,
        uint64_t uid,
        std::shared_ptr<GfxDevice> device);
    ~RenderActor();

    /** @brief Updates the per object descriptor set and binds. */
    virtual void OnRender(RenderData& rd);

private:
    void UpdateObjectUBO(int currentFrame);
    
    ObjectUBO _objectData;
    std::array<VkDescriptorSet, GfxLimits::maxFramesInFlight> _perFrameSets;
    std::unique_ptr<GfxBuffer> _objectDUBO;
};

} // namespace bl