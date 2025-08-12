#include "Shader.h"

namespace bl {

Shader::Shader(ResourceManager* manager, VulkanDevice* device)
    : Resource(manager)
    , _device(device)
{
}

bool Shader::Load()
{
    std::vector<uint32_t> code;

    switch (GetLocation()) {
    case ResourceSource::eFile: {
        std::ifstream file(GetPath(), std::ios::in | std::ios::binary);
        if (!file.good()) {
            Log::Error("Could not open shader file!");
            return false;
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.read(0, std::ios::beg);
        
        code.resize(size);

        file.read(reinterpret_cast<char*>(code.data()), size);
        break;
    }
    case ResourceSource::eBinary: {
        auto data = GetBinaryData();
        code.resize(data.size());
        std::copy(data.begin(), data.end(), code.begin());
    }
    }

    if (code.size() % 4 != 0) {
        Log::Error("Code byte size must be divisible by 4 for valid SPIR-V code!");
        return false;
    }

    _shader = std::make_unique<VulkanShader>(_device, code);
}

void Shader::Unload()
{
    _shader.release();
}

bool Shader::ExportBinary(std::ostream& stream) const
{

}

}