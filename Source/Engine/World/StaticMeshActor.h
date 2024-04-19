#pragma once

#include "RenderActor.h"

namespace bl
{

class StaticMeshActor : public RenderActor
{
public:
    StaticMeshActor(
        const std::string& name,
        uint64_t uid);
    ~StaticMeshActor();

    void SetMaterial(std::shared_ptr<MaterialInstance> material);
    void SetMesh(std::shared_ptr<Mesh> mesh);

private:
    std::shared_ptr<MaterialInstance> _material;
    std::shared_ptr<Mesh> _mesh;
};

} // namespace bl