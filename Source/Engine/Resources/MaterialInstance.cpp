#include "MaterialInstance.h"
#include "Engine/Engine.h"
#include "Graphics/GraphicsSystem.h"
#include "Material.h"
#include "Graphics/Renderer.h"
#include "ResourceSystem.h"
#include "Graphics/VulkanReflectedBlock.h"
#include "Core/Reflection/NamedProperty.h"
#include "Core/ClassDB.h"
#include "Resources/Resource.h"

namespace bl {

MaterialInstance::MaterialInstance()
    : Resource()
    , _materialInstance(nullptr)
{
    // Empty constructor for creating material instances from a base material.
}

MaterialInstance::MaterialInstance(std::unique_ptr<VulkanMaterialInstance> instance)
    : Resource("")
    , _renderer(GraphicsSystem::Get()->GetRenderer())
    , _materialInstance(std::move(instance))
{
    // Since the material instance now owns this, it's responsible
    // for adding and removing the material from the renderer.
    _renderer->AddMaterial(_materialInstance.get());

    RegisterMaterialProperties(_materialInstance.get());
}

MaterialInstance::MaterialInstance(const std::filesystem::path& path)
    : Resource(path)
    , _renderer(GraphicsSystem::Get()->GetRenderer())
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

    auto resourceSystem = ResourceSystem::Get();
    auto mat = resourceSystem->Load<Material>(json["material"].get<std::string>());
    _materialInstance = mat.lock()->GetVulkanMaterial()->CreateInstance();

    // Ensure that the material buffers get properly cleaned updated every frame.
    _renderer->AddMaterial(_materialInstance.get());

    RegisterMaterialProperties(_materialInstance.get());

}

MaterialInstance::MaterialInstance(const MaterialInstance&)
    : Resource("")
{
    // TODO: Implement copy constructor to properly copy the material instance and its properties.
}

MaterialInstance::~MaterialInstance()
{
    if (_materialInstance)
        _renderer->RemoveMaterial(_materialInstance.get());
}

void MaterialInstance::Release()
{
    Resource::Release();

    if (_materialInstance)
        _renderer->RemoveMaterial(_materialInstance.get());

    _materialInstance.reset();
}

VulkanMaterialInstance* MaterialInstance::GetInstance() const
{
    return _materialInstance.get();
}

Variant MaterialInstance::GetMaterialProperty(std::string_view name)
{
    // Make sure the uniform exists in the material, and then get it using the material instance.
    const auto& uniforms = _materialInstance->GetBaseMaterial()->GetUniforms();
    auto it = uniforms.find(std::string{name});
    if (it == uniforms.end()) {
        Print::Error("Could not get material uniform, it does not exist!");
        return Variant{};
    }

    // Get the uniform block for this uniform and get it using the material instance.
    Variant value;
    switch (it->second.GetType())
    {
    case VulkanVariableBlockType::eScalarBool: {
        int32_t v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = static_cast<bool>(v);
        break;
    }
    case VulkanVariableBlockType::eScalarInt: {
        int v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    case VulkanVariableBlockType::eScalarFloat: {
        float v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    case VulkanVariableBlockType::eVector2: {
        glm::vec2 v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    case VulkanVariableBlockType::eVector3: {
        glm::vec3 v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    case VulkanVariableBlockType::eVector4: {
        glm::vec4 v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    case VulkanVariableBlockType::eMatrix4: {
        glm::mat4 v;
        _materialInstance->GetGenericUniform(name.data(), v);
        value = v;
        break;
    }
    default:
        Print::Error("Unsupported uniform type for material uniform {}!", name);
        break;
    }

    return value;
}

void MaterialInstance::RegisterMaterialProperties(VulkanMaterialInstance* materialInstance)
{
    // Add properties from materials.
    const auto& uniforms = materialInstance->GetBaseMaterial()->GetUniforms();
    for (const auto& uniform : uniforms) {

        VariantType type = VariantType::eInteger;
        switch (uniform.second.GetType())
        {        
        case VulkanVariableBlockType::eScalarBool: type = VariantType::eBoolean; break;
        case VulkanVariableBlockType::eScalarInt: type = VariantType::eInteger; break;
        case VulkanVariableBlockType::eScalarFloat: type = VariantType::eFloat; break;
        case VulkanVariableBlockType::eVector2: type = VariantType::eVector2; break;
        case VulkanVariableBlockType::eVector3: type = VariantType::eVector3; break;
        case VulkanVariableBlockType::eVector4: type = VariantType::eVector4; break;
        case VulkanVariableBlockType::eMatrix4: type = VariantType::eMatrix4; break;
        default: continue; break;
        }

        // If the first character contains a 'c[UPPER]', then it's a color
        const auto& name = uniform.first;
        PropertyFlags flags = PropertyFlags::None;
        if (type == VariantType::eVector4 && name.size() > 9 && name[9] == 'c' && std::isupper(name[10])) {
            flags |= PropertyFlags::Color;
        }

        AddInstanceProperty(std::make_unique<TNamedProperty<MaterialInstance>>(uniform.second.GetName(), type, PropertyFlags::Editor | PropertyFlags::Serialize | flags, &MaterialInstance::SetMaterialProperty, &MaterialInstance::GetMaterialProperty));
    }
}

void MaterialInstance::SetMaterialProperty(std::string_view name, const Variant& value)
{
    // Make sure the uniform exists in the material, and then set it using the material instance.
    const auto& uniforms = GetInstance()->GetBaseMaterial()->GetUniforms();
    auto it = uniforms.find(std::string{name});
    if (it == uniforms.end()) {
        Print::Error("Could not set material uniform, it does not exist!");
        return;
    }

    // Make sure the types are the same.
    VariantType type = VariantType::eInteger;
    switch (it->second.GetType())
    {
    case VulkanVariableBlockType::eScalarBool: type = VariantType::eBoolean; break;
    case VulkanVariableBlockType::eScalarInt: type = VariantType::eInteger; break;
    case VulkanVariableBlockType::eScalarFloat: type = VariantType::eFloat; break;
    case VulkanVariableBlockType::eVector2: type = VariantType::eVector2; break;
    case VulkanVariableBlockType::eVector3: type = VariantType::eVector3; break;
    case VulkanVariableBlockType::eVector4: type = VariantType::eVector4; break;
    case VulkanVariableBlockType::eMatrix4: type = VariantType::eMatrix4; break;
    default:
        Print::Error("Unsupported uniform type for material uniform {}!", name);
        break;
    }

    if (value.index() != static_cast<std::size_t>(type)) {
        Print::Error("Could not set material uniform, type mismatch!");
        return;
    }

    // Get the uniform block for this uniform and set it using the material instance.
    std::visit([&](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, bool>) {
            SetInteger(name.data(), static_cast<int32_t>(arg));
        } else if constexpr (std::is_same_v<T, int>) {
            SetInteger(name.data(), arg);
        } else if constexpr (std::is_same_v<T, float>) {
            SetScaler(name.data(), arg);
        } else if constexpr (std::is_same_v<T, glm::vec2>) {
            SetVector2(name.data(), arg);
        } else if constexpr (std::is_same_v<T, glm::vec3>) {
            SetVector3(name.data(), arg);
        } else if constexpr (std::is_same_v<T, glm::vec4>) {
            SetVector4(name.data(), arg);
        } else if constexpr (std::is_same_v<T, glm::mat4>) {
            SetMatrix(name.data(), arg);
        } else {
            Print::Error("Unsupported uniform type for material uniform {}!", name);
        }
    }, value);
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

void MaterialInstance::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("MaterialInstance", "Resource", &MaterialInstance::Create);
}

} // namespace bl