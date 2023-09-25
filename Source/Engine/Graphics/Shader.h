#pragma once

#include "Device.h"

namespace bl {

struct ShaderCreateInfo {
    GraphicsDevice*              pDevice;   /** @brief Graphics device to create this shader on. */
    VkShaderStageFlagBits        stage;     /** @brief Stage type i.e. Vertex, Fragment. */
    const std::vector<uint32_t>& binary;    /** @brief Binary SPIR-V data from a shader file or resource. */
};

/** @brief A single unit of a shader pipeline. */
class BLUEMETAL_API Shader {
public:

    /** @brief Default Constructor */
    Shader();

    /** @brief Move Constructor */
    Shader(Shader&& rhs);

    /** @brief Create Constructor */
    Shader(const ShaderCreateInfo& info);

    /** @brief Default Destructor */
    ~Shader();

    /** @brief Creates this shader object. */
    [[nodiscard]] bool create(const ShaderCreateInfo& info) noexcept; 

    /** @brief Destroys this shader object. */
    void destroy() noexcept;

    /** @brief Returns true if this shader was created. */
    [[nodiscard]] bool isCreated() const noexcept;

public:

    /** @brief Returns the shader stage created with. */
    VkShaderStageFlagBits getStage() const noexcept;

    /** @brief Returns the underlying shader module handle. */
    VkShaderModule getHandle() const noexcept;

    /** @brief Returns a ref to the original binary used to create this shader. */
    const std::vector<uint32_t>& getBinary() const noexcept;

private:
    bool createModule() noexcept;

    GraphicsDevice*         _pDevice;
    VkShaderStageFlagBits   _stage;
    std::vector<uint32_t>   _binary;
    VkShaderModule          _module;
};

} // namespace bl
