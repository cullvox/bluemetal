
#pragma once

#include "Node3D.h"
#include "MaterialInstance.h"

class MeshInstance3D : public Node3D {

    Ref<MaterialIntance> _material;
    Ref<Mesh> _mesh;

public:
    MeshInstance3D() = default;
    virtual ~MeshInstance3D() = default;

    virtual void Draw() override; // Just sets up the instance data, actual draw call is done in renderer

    void SetMesh(Ref<Mesh> mesh);
    void SetMaterial(Ref<MaterialInstance> material);
    Ref<Mesh> GetMesh() const;
    Ref<MaterialIntance> GetMaterial() const;
};