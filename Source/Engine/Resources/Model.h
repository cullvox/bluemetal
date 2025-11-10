#pragma once

#include "Precompiled.h"

#include "Resources/Resource.h"
#include "Resources/Texture2D.h"
#include "Resources/MaterialInstance.h"
#include "Resources/Mesh.h"

#include "Scene/Node3D.h"

namespace bl
{

class Model : public Resource
{
    std::vector<Ref<Mesh>> _meshes;
    std::vector<Ref<MaterialInstance>> _materials;
    std::vector<Ref<Texture2D>> _textures;
    std::shared_ptr<Node3D> _root;

public:
    Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~Model();

    std::shared_ptr<Node3D> GetTree();
};

}