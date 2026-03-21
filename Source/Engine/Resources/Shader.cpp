#include "Shader.h"
#include "Engine/Engine.h"

namespace bl {

Shader::Shader(Engine& engine, const std::filesystem::path& path)
    : Resource(engine, path)
    , _device(engine.GetGraphics().GetDevice())
{
    std::vector<uint32_t> code;

    const auto fullPath = std::filesystem::current_path() / path;
    std::ifstream file(fullPath, std::ios::in | std::ios::binary);
    if (!file.good()) {
        throw std::runtime_error("Could not open shader file!");
    }

    code.resize(std::filesystem::file_size(fullPath));

    file.read(reinterpret_cast<char*>(code.data()), code.size());

    if (code.size() % 4 != 0) {
        throw std::runtime_error("Code byte size must be divisible by 4 for valid SPIR-V code!");
    }

    _shader = std::make_unique<VulkanShader>(engine.GetGraphics().GetDevice(), code);
}

Shader::~Shader()
{
}

}