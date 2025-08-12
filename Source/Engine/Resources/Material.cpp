#include "Material.h"
#include "Graphics/VulkanShader.h"
#include "Renderer.h"
#include "Resource/ResourceManager.h"

namespace bl {

Material::Material(ResourceManager* manager, const nlohmann::json& data, VulkanDevice* device, Renderer* renderer)
    : Resource(manager, data)
    , _renderer(renderer)
    , _device(device)
{
}

Material::~Material()
{
}

bool Material::Load()
{
    std::ifstream materialFile(GetFilePath());

    std::string vertexPath, fragmentPath;
    nlohmann::json json;
    VulkanPipelineStateInfo info;
    RenderPassType passType;

    try {
        json = nlohmann::json::parse(materialFile);

        passType = json["renderPass"];
        vertexPath = json["shaders"]["vertex"].get<std::string>();
        fragmentPath = json["shaders"]["fragment"].get<std::string>();

        info = json["state"];

        auto vertexShader = GetResourceManager()->Load<bl::VulkanShader>(vertexPath);
        auto fragmentShader = GetResourceManager()->Load<bl::VulkanShader>(fragmentPath);
        info.stages.shaders = { vertexShader, fragmentShader };
    } catch (const std::exception& e) {
        Log::Error("Could not parse material json: {}", e.what());
        return false;
    }

    auto [pass, subpass] = _renderer->GetRenderPass(passType);

    _material = std::make_unique<VulkanMaterial>(_device, pass, subpass, info, _renderer->GetSwapchainImageCount());

    _renderer->AddMaterial(this); // Ensures that the material buffers get properly cleaned updated every frame.
}

void Material::Unload()
{
    _renderer->RemoveMaterial(this);
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

void Material::SetSampledImage2D(const std::string& name, VulkanSampler* sampler, VulkanImage* image)
{
    _material->SetSampledImage2D(name, sampler, image);
}

void Material::UpdateUniforms()
{
    _material->UpdateUniforms();
}

void Material::Bind(VulkanRenderData& rd)
{
    _material->Bind(rd);
}

void Material::PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value)
{
    _material->PushConstant(rd, offset, size, value);
}

const VulkanPipeline* Material::GetPipeline()
{
    return _material->GetPipeline();
}

}