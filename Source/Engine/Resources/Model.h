#pragma once

#include <tiny_gltf.h>

#include "Precompiled.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanRenderData.h"
#include "Graphics/VulkanSampler.h"
#include "Resource.h"
#include "StaticMesh.h"

#include "Scene/Node3D.h"
#include "Mesh.h"
#include "MaterialInstance.h"

namespace bl
{

class Model : public Resource
{
    std::vector<Ref<Mesh>> _meshes;
    std::vector<Ref<MaterialInstance>> _materials;
    std::vector<Ref<Texture2D>> _textures;
    std::shared_ptr<Node3D> _root;

    std::shared_ptr<Node3D> LoadNode(const tinygltf::Model& model, const tinygltf::Node& node, GraphicsSystem* system);

public:
    Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~Model();

    std::shared_ptr<Node3D> GetTree();
};

}