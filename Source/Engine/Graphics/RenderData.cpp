#include "RenderData.h"


namespace bl {

void RenderData::IncrementCurrentFrame() {
    
}

void RenderData::DrawInstance(MaterialInstance* material, Mesh* mesh, const InstanceData& instance)
{
    auto pair = std::make_pair(material, mesh);
    auto& call = _calls[pair];

    if (call.count > 1000) {
        return;
    }

    if (call.instances.max_size() != 1000) {
        call.instances.resize(1000);
    }

    call.instances.push_back(instance);
    call.count++;
}

void RenderData::Execute(VkCommandBuffer cmd)
{
    
}

} // namespace bl