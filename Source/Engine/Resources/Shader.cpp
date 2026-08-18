#include "Shader.h"
#include "Engine/Engine.h"

namespace bl {

Shader::Shader()
    : Resource()
{
}

Shader::Shader(const std::filesystem::path& path)
    : Resource(path)
{
}

Shader::Shader(const Shader& copy)
    : _device(copy._device)
{
    throw std::runtime_error("Cannot copy a shader yet.");
}

Shader::~Shader() = default;

void Shader::Load()
{
    auto& json = GetJson();

    std::vector<uint32_t> code;

    const auto fullPath = std::filesystem::current_path() / json["path"].get<std::string>();;
    std::ifstream file(fullPath, std::ios::in | std::ios::binary);
    if (!file.good()) {
        throw std::runtime_error("Could not open shader file!");
    }

    code.resize(std::filesystem::file_size(fullPath));

    file.read(reinterpret_cast<char*>(code.data()), code.size());

    if (code.size() % 4 != 0) {
        throw std::runtime_error("Code byte size must be divisible by 4 for valid SPIR-V code!");
    }

    _shader = std::make_unique<VulkanShader>(GraphicsSystem::Get()->GetDevice(), code);
}

void Shader::Release()
{
    Resource::Release();
    _shader.reset();
}

void Shader::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Shader", "Resource", &Shader::Create);
}

}