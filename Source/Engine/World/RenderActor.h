#pragma once

#include "Graphics/Config.h"
#include "Graphics/Buffer.h"
#include "Actor.h"

namespace bl
{

class RenderActor : public Actor
{
public:
    RenderActor(const std::string& name, uint64_t uid, Device* device);
    ~RenderActor();

    virtual void OnRender(RenderData& rd); /** @brief Updates the per object descriptor set and binds. */

private:
    void UpdateObjectUBO(int currentFrame);
    
    ObjectPC _objectData;
    std::array<VkDescriptorSet, GraphicsConfig::numFramesInFlight> _perFrameSets;
    std::unique_ptr<Buffer> _objectDUBO;
};

} // namespace bl