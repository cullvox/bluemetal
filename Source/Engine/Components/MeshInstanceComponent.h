#pragma once

#include "World/Component.h"

namespace bl
{

class StaticMeshComponent
{
public:
    StaticMeshComponent();
    ~StaticMeshComponent();

    void SetMaterial(MaterialInstance* material);
    void SetMesh(Mesh* mesh);

private:
    MaterialInstance* _material;
    Mesh* _mesh;
};

} // namespace bl