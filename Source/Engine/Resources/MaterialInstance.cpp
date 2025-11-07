#include "MaterialInstance.h"
#include "Material.h"
#include "Engine/Engine.h"

namespace bl
{

MaterialInstance::MaterialInstance(ResourceSystem* resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path)
    : Resource(resourceSystem, graphicsSystem, path)
{
    std::ifstream file{path};
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open material instance JSON file.");
    }

    nlohmann::json json;
    try 
    {
        json = nlohmann::json::parse(file);
        // TODO: Parse the JSON and set up the material instance.
    }
    catch (...)
    {
        throw std::runtime_error("Could not parse material JSON file.");
    }

    auto mat = resourceSystem->Load<Material>(json["material"].get<std::string>());
    _materialInstance = std::unique_ptr<VulkanMaterialInstance>(mat->GetVulkanMaterial()->CreateInstance());
}

MaterialInstance::~MaterialInstance()
{
}

void MaterialInstance::SetBool(const std::string& name, bool value)
{
    _materialInstance->SetBool(name, value);
}

void MaterialInstance::SetInteger(const std::string& name, int value)
{
    _materialInstance->SetInteger(name, value);
}

void MaterialInstance::SetScaler(const std::string& name, float value)
{
    _materialInstance->SetScaler(name, value);
}

void MaterialInstance::SetVector2(const std::string& name, glm::vec2 value)
{
    _materialInstance->SetVector2(name, value);
}

void MaterialInstance::SetVector3(const std::string& name, glm::vec3 value)
{
    _materialInstance->SetVector3(name, value);
}

void MaterialInstance::SetVector4(const std::string& name, glm::vec4 value)
{
    _materialInstance->SetVector4(name, value);
}

void MaterialInstance::SetMatrix(const std::string& name, glm::mat4 value)
{
    _materialInstance->SetMatrix(name, value);
}

void MaterialInstance::SetSampledTexture(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image)
{
    _materialInstance->SetSampledImage2D(name, sampler->GetSampler(), image->GetImage());
}

void MaterialInstance::UpdateUniforms()
{
    _materialInstance->UpdateUniforms();
}

void MaterialInstance::Bind(VulkanRenderData& rd)
{
    _materialInstance->Bind(rd);
}

void MaterialInstance::PushConstant(VulkanRenderData& rd, uint32_t offset, uint32_t size, const void* value)
{
    _materialInstance->PushConstant(rd, offset, size, value);
}


} // namespace bl