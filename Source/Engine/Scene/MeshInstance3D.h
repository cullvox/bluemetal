
#pragma once

#include "Node3D.h"
#include "Resources/MaterialInstance.h"

class MeshInstance3D : public Node3D {
    OBJECT_CLASS(MeshInstance3D, Node3D)
public:
    MeshInstance3D() = default;
    virtual ~MeshInstance3D() = default;

    void SetMesh(Ref<Mesh> mesh);
    void SetMaterial(Ref<MaterialInstance> material);
    virtual void Draw() override;
};