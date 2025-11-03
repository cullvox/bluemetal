#include <tiny_gltf.h>

#include "Graphics/UniformData.h"
#include "Core/FileByte.h"
#include "Core/Print.h"
#include "Graphics/ModelFormat.h"
#include "Graphics/VulkanImage.h"
#include "Graphics/VulkanMaterialInstance.h"
#include "Graphics/VulkanSampler.h"
#include "Texture2D.h"
#include "Model.h"

namespace bl
{

Model::Model(ResourceSystem* resourceSystem, GraphicsSystem* system, const std::filesystem::path& path)
    : Resource(resourceSystem, system, path)
{
    tinygltf::TinyGLTF loader;
    tinygltf::Model model;
    std::string err;
    std::string warn;

    bool res = loader.LoadASCIIFromFile(&model, &err, &warn, path.string());

    if (!warn.empty())
        Print::Warn("GLTF Load: {}", warn);

    if (!err.empty())
        Print::Error("GLTF Load: {}", err);

    if (!res)
        throw std::runtime_error("Could not load a model file!");

    // Load meshes
    const auto& scene = model.scenes[model.defaultScene];

    for (int i : scene.nodes)
    {
    //    model.nodes[i].mesh
    }

}

Model::~Model()
{
}

void Model::Draw(VulkanRenderData&, VulkanMaterialInstance*)
{
    for (int i = 0; i < (int)_meshes.size(); i++)
    {

    }
}

}