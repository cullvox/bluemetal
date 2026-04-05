#pragma once

#include "Resources/Resource.h"

namespace tinygltf {
class Model;
class Node;
}

namespace bl {

class Mesh;
class MaterialInstance;
class Texture2D;
class Node3D;

class Model : public Resource {
    GraphicsSystem* _graphicsSystem;
    std::vector<Ref<Mesh>> _meshes; // For every primitive, for each mesh
    std::vector<Ref<MaterialInstance>> _materials;
    std::vector<Ref<Texture2D>> _textures;
    std::unique_ptr<Node3D> _root;

    std::unique_ptr<Node3D> LoadNode(const tinygltf::Model& model, const tinygltf::Node& node);

public:
    Model(Engine& engine, const std::filesystem::path& path);
    ~Model();

    Node3D* GetTree();
    const std::vector<Ref<Mesh>>& GetMeshes() const;
};

}