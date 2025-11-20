#pragma once

#include "Precompiled.h"

#include "Resources/MaterialInstance.h"
#include "Resources/Mesh.h"
#include "Resources/Resource.h"
#include "Resources/Texture2D.h"

#include "Scene/Node3D.h"

namespace tinygltf {
class Model;
class Node;
}

namespace bl {

class Model : public Resource {
    GraphicsSystem* _graphicsSystem;
    std::vector<Ref<Mesh>> _meshes; // For every primitive, for each mesh
    std::vector<Ref<MaterialInstance>> _materials;
    std::vector<Ref<Texture2D>> _textures;
    std::shared_ptr<Node3D> _root;

    std::shared_ptr<Node3D> LoadNode(const tinygltf::Model& model, const tinygltf::Node& node);

public:
    Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~Model();

    std::shared_ptr<Node3D> GetTree();
};

}