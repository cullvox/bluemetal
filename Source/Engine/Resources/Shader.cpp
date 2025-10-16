#include "Shader.h"
#include "Engine/Engine.h"

namespace bl 
{

Shader::Shader(const std::filesystem::path& path)
{
    std::vector<uint32_t> code;

    std::ifstream file(path, std::ios::in | std::ios::binary);
    if (!file.good()) 
    {
        throw std::runtime_error("Could not open shader file!");
    }
    
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    file.read(0, std::ios::beg);
    
    code.resize(size);

    file.read(reinterpret_cast<char*>(code.data()), size);

    if (code.size() % 4 != 0) 
    {
        throw std::runtime_error("Code byte size must be divisible by 4 for valid SPIR-V code!");
    }

    _shader = std::make_unique<VulkanShader>(GetEngine()->GetGraphics()->GetDevice(), code);
}

Shader::~Shader()
{
}

}