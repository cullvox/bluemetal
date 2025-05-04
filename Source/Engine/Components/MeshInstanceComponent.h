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
    void SetMesh(StaticMesh* mesh);

private:
    MaterialInstance* _material;
    StaticMesh* _mesh;
};

} // namespace bl