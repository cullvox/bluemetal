
#pragma once

#include "Node3D.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Mesh.h"

namespace bl {

class MeshInstance3D : public Node3D {
    Ref<MaterialInstance> _material;
    Ref<Mesh> _mesh;

public:
    MeshInstance3D(Engine& engine);
    MeshInstance3D(const MeshInstance3D&);
    virtual ~MeshInstance3D() = default;

    virtual MeshInstance3D* Clone() override;
    virtual void Draw(VulkanRenderData& rd) override; // Just sets up the instance data, actual draw call is done in renderer

    void SetMesh(Ref<Mesh> mesh);
    void SetMaterial(Ref<MaterialInstance> material);
    Ref<Mesh> GetMesh() const;
    Ref<MaterialInstance> GetMaterial() const;
};

} // namespace bl