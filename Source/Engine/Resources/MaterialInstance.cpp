#include "MaterialInstance.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Material.h"
#include "ResourceSystem.h"

namespace bl {

MaterialInstance::MaterialInstance(ResourceSystem& resourceSystem, GraphicsSystem* graphicsSystem)
    : Resource(resourceSystem, graphicsSystem, "")
    , _renderer(graphicsSystem->GetRenderer())
    , _materialInstance(nullptr)
{
    // Empty constructor for creating material instances from a base material.
}

MaterialInstance::MaterialInstance(ResourceSystem& resourceSystem, GraphicsSystem* graphicsSystem, std::unique_ptr<VulkanMaterialInstance> instance)
    : Resource(resourceSystem, graphicsSystem, "")
    , _renderer(graphicsSystem->GetRenderer())
    , _materialInstance(std::move(instance))
{
    // Since the material instance now owns this, it's responsible
    // for adding and removing the material from the renderer.
    _renderer->AddMaterial(_materialInstance.get());
}

MaterialInstance::MaterialInstance(ResourceSystem& resourceSystem, GraphicsSystem* graphicsSystem, const std::filesystem::path& path)
    : Resource(resourceSystem, graphicsSystem, path)
    , _renderer(graphicsSystem->GetRenderer())
    , _materialInstance(nullptr)
{
    std::ifstream file { path };
    if (!file.is_open()) {
        throw std::runtime_error("Could not open material instance JSON file.");
    }

    nlohmann::json json;
    try {
        json = nlohmann::json::parse(file);
        // TODO: Parse the JSON and set up the material instance.
    } catch (...) {
        throw std::runtime_error("Could not parse material JSON file.");
    }

    auto mat = resourceSystem.Load<Material>(json["material"].get<std::string>());
    _materialInstance = std::unique_ptr<VulkanMaterialInstance>(mat.lock()->GetVulkanMaterial()->CreateInstance());

    // Ensure that the material buffers get properly cleaned updated every frame.
    _renderer->AddMaterial(_materialInstance.get());
}

MaterialInstance::~MaterialInstance()
{
    if (_materialInstance)
        _renderer->RemoveMaterial(_materialInstance.get());
}

VulkanMaterialInstance* MaterialInstance::GetInstance() const
{
    return _materialInstance.get();
}

void MaterialInstance::SetBool(const std::string& name, bool value)
{
    GetInstance()->SetBool(name, value);
}

void MaterialInstance::SetInteger(const std::string& name, int value)
{
    GetInstance()->SetInteger(name, value);
}

void MaterialInstance::SetScaler(const std::string& name, float value)
{
    GetInstance()->SetScaler(name, value);
}

void MaterialInstance::SetVector2(const std::string& name, glm::vec2 value)
{
    GetInstance()->SetVector2(name, value);
}

void MaterialInstance::SetVector3(const std::string& name, glm::vec3 value)
{
    GetInstance()->SetVector3(name, value);
}

void MaterialInstance::SetVector4(const std::string& name, glm::vec4 value)
{
    GetInstance()->SetVector4(name, value);
}

void MaterialInstance::SetMatrix(const std::string& name, glm::mat4 value)
{
    GetInstance()->SetMatrix(name, value);
}

void MaterialInstance::SetSampledTexture2D(const std::string& name, Ref<Sampler> sampler, Ref<Texture> image)
{
    GetInstance()->SetSampledImage2D(name, sampler.lock()->GetSampler(), image.lock()->GetImage());
}

void MaterialInstance::Bind(RenderData& rd)
{
    GetInstance()->Bind(rd);
}

void MaterialInstance::PushConstant(RenderData& rd, uint32_t offset, uint32_t size, const void* value)
{
    GetInstance()->PushConstant(rd, offset, size, value);
}

} // namespace bl