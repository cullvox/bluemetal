#include "Material.h"
#include "Engine/Engine.h"
#include "Shader.h"
#include "Graphics/VulkanShader.h"
#include "Graphics/Renderer.h"

namespace bl {

Material::Material(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path)
    : MaterialInstance(resourceSystem, graphicsSystem, path)
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
        info.stages.shaders = std::vector<VulkanShader*>{ vertexShader.Get()->Get(), fragmentShader.Get()->Get() };
    }
    catch (...)
    {
        throw std::runtime_error("Could not parse material JSON file.");
    }

    auto [pass, subpass] = graphicsSystem->GetRenderer()->GetRenderPass(passType);

    _material = std::make_unique<VulkanMaterial>(graphicsSystem->GetDevice(), pass, subpass, info, _renderer->GetSwapchainImageCount());

    _renderer->AddMaterial(_material.get()); // Ensures that the material buffers get properly cleaned updated every frame.
}

Material::~Material()
{
    _renderer->RemoveMaterial(_material.get());
}

void Material::SetBool(const std::string& name, bool value)
{
    _material->SetBool(name, value);
}

void Material::SetInteger(const std::string& name, int value)
{
    _material->SetInteger(name, value);
}

void Material::SetScaler(const std::string& name, float value)
{
    _material->SetScaler(name, value);
}

void Material::SetVector2(const std::string& name, glm::vec2 value)
{
    _material->SetVector2(name, value);
}

void Material::SetVector3(const std::string& name, glm::vec3 value)
{
    _material->SetVector3(name, value);
}

void Material::SetVector4(const std::string& name, glm::vec4 value)
{
    _material->SetVector4(name, value);
}

void Material::SetMatrix(const std::string& name, glm::mat4 value)
{
    _material->SetMatrix(name, value);
}

void Material::SetSampledImage2D(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image)
{
    _material->SetSampledImage2D(name, sampler.Get()->GetSampler(), image.Get()->GetImage());
}

const VulkanPipeline* Material::GetVulkanPipeline()
{
    return _material->GetPipeline();
}

}