#pragma once

#include "Graphics/Config.h"
#include "Graphics/Resource/Buffer.h"
#include "Actor.h"

namespace bl
{

class RenderActor : public Actor
{
public:
    RenderActor(const std::string& name, uint64_t uid, std::shared_ptr<GfxDevice> device);
    ~RenderActor();

    virtual void OnRender(RenderData& rd); /** @brief Updates the per object descriptor set and binds. */

private:
    void UpdateObjectUBO(int currentFrame);
    
    ObjectUBO _objectData;
    std::array<VkDescriptorSet, GraphicsConfig::maxFramesInFlight> _perFrameSets;
    std::unique_ptr<Buffer> _objectDUBO;
};

} // namespace bl