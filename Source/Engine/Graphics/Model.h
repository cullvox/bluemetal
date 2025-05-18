#pragma once

#include "Graphics/Material.h"
#include "Graphics/VulkanDevice.h"
#include "Graphics/VulkanRenderData.h"
#include "Resource/Resource.h"
#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/scene.h>
#include "Mesh.h"

namespace bl
{

class Model : public Resource
{
public:
    Model(const nlohmann::json& data, VulkanDevice* device);
    ~Model();

    virtual void Load() override;
    virtual void Unload() override;

    void Draw(MaterialInstance* material, VulkanRenderData& rd);

private:
    void ProcessNodes(const aiNode* node, const aiScene* scene);
    void ProcessMesh(const aiMesh* node, const aiScene* scene);

    VulkanDevice* _device;
    std::vector<StaticMesh> _meshes;
    std::vector<glm::mat4> _transforms;
    std::vector<int> _meshTransformIndicies;
};

}