#include "Core/Log.hpp"
#include "Core/Macros.hpp"
#include "Render/Shader.hpp"


blShader::blShader(const RenderDevice* renderDevice,
    const std::vector<uint32_t>& spirvBytes) noexcept
    : _renderDevice(renderDevice)
{
    const VkShaderModuleCreateInfo createInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    
}

Shader::~Shader() noexcept
{
    collapse();
}

Shader& Shader::operator=(Shader&& rhs) noexcept
{
    this->collapse();

    m_pRenderDevice = rhs.m_pRenderDevice;

    rhs.collapse();
    return *this;
}

bool Shader::good() const noexcept
{
    return m_pRenderDevice != nullptr;
}


void Shader::collapse() noexcept
{
   
}

bool Shader::generateReflectShaderModules() noexcept
{
    
    // Load the shader files into a buffer and parse each of them.
    SpvReflectShaderModule shaderModule{};

    for (const auto& path : m_shaderPaths)
    {
        std::ifstream infile{ path };
        BL_CHECK_GOTO(
            infile, 
            "Could not open a shader file!", 
            failureCleanup)

        std::string contents{ std::istreambuf_iterator<char>{infile}, std::istreambuf_iterator<char>{} };
        
        BL_CHECK_GOTO(
            spvReflectCreateShaderModule(
                contents.size(),
                reinterpret_cast<uint32_t*>(contents.data()),
                &shaderModule) == SPV_REFLECT_RESULT_SUCCESS,
            "Could not reflect a shader module!",
            failureCleanup)

        m_reflectShaderModules.emplace_back(shaderModule);
    }

    return true;

failureCleanup:
    m_reflectShaderModules.clear();
    return false;
}

void Shader::generateDescriptorBindings() noexcept
{



}

void Shader::generateShaderStages() noexcept
{

}

} // namespace bl