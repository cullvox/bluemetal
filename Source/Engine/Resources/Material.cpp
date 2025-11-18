#include "Material.h"
#include "Engine/Engine.h"
#include "Shader.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/Renderer.h"

namespace bl {

Material::Material(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path)
    : MaterialInstance(resourceSystem, graphicsSystem)
    , _resourceSystem(resourceSystem)
    , _graphicsSystem(graphicsSystem)
    , _renderer(graphicsSystem->GetRenderer())
{
    std::ifstream materialFile{path};
    if (!materialFile.is_open()) 
    {
        throw std::runtime_error("Could not open material JSON file.");
    }

    std::string vertexPath, fragmentPath;
    nlohmann::json json;
    VulkanPipelineStateInfo info;
    RenderPassType passType;

    try 
    {
        json = nlohmann::json::parse(materialFile);

        passType = json["renderPass"];
        vertexPath = json["shaders"]["vertex"].get<std::string>();
        fragmentPath = json["shaders"]["fragment"].get<std::string>();

        info = json["state"];

        auto vertexShader = resourceSystem->Load<Shader>(vertexPath);
        auto fragmentShader = resourceSystem->Load<Shader>(fragmentPath);
        info.stages.shaders = std::vector<VulkanShader*>{ vertexShader->Get(), fragmentShader->Get() };
    }
    catch (...)
    {
        throw std::runtime_error("Could not parse material JSON file.");
    }

    auto [pass, subpass] = _renderer->GetRenderPass(passType);

    auto newMat = std::make_unique<VulkanMaterial>(graphicsSystem->GetDevice(), pass, subpass, info, _renderer->GetSwapchainImageCount());
    _material = newMat.get();

    _materialInstance = std::unique_ptr<VulkanMaterialInstance>(newMat.release());

    _renderer->AddMaterial(_material); // Ensures that the material buffers get properly cleaned updated every frame.
}

Material::~Material()
{
    _renderer->RemoveMaterial(_material);
}

Ref<MaterialInstance> Material::CreateInstance()
{
    return std::make_shared<MaterialInstance>(_resourceSystem, _graphicsSystem, _material->CreateInstance());
}

const VulkanPipeline* Material::GetVulkanPipeline()
{
    return _material->GetPipeline();
}

VulkanMaterial* Material::GetVulkanMaterial()
{
    return _material;
}

}