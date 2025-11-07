
#pragma once

#include "Node3D.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Mesh.h"

namespace bl
{

class MeshInstance3D : public Node3D
{
    Ref<Material> _material;
    Ref<Mesh> _mesh;

public:
    MeshInstance3D(Engine* engine);
    virtual ~MeshInstance3D() = default;

    virtual void Draw(VulkanRenderData& rd) override; // Just sets up the instance data, actual draw call is done in renderer

    void SetMesh(Ref<Mesh> mesh);
    void SetMaterial(Ref<Material> material);
    Ref<Mesh> GetMesh() const;
    Ref<Material> GetMaterial() const;
};

} // namespace bl