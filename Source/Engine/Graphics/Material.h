#pragma once

#include "Resource/Resource.h"
#include "DescriptorSet.h"

namespace bl
{

enum class MaterialPass
{
    eGeometry, /* Uses geometry pass (unlit) */
    eLit, /* Uses the lighting pass (lit) */
}

/** @brief Shader uniforms merged together in an uncomfortably useful unison.
 * 
 * A brief explanation of descriptor sets in materials.
 *
 * Materials are limited to using descriptor set index one. Therefore
 * bindings and all material data are all inside of one set and limited.
 * Many material instances can be created from one material allowing
 * variation with material uniforms in a simple and effectively understandable
 * way.
 *
 * The renderer is supposed to handle descriptor sets zero and two for 
 * global and instance rendering data respectively. 
 * 
 */
class Material : public Resource
{
public:
    Material(const nlohmann::json& data);
    ~Material();

    virtual void Load();
    virtual void Unload();

    std::unique_ptr<MaterialInstance> CreateInstance();

private:
    std::unique_ptr<DescriptorSetCache> _descriptorSetCache;
    std::unique_ptr<Pipeline> pipeline;
    std::unordered_map<BlockReflection> _uniformBufferBlocks;
    std::unordered_map<BlockReflection> _pushConstantBlocks;
    VkDescriptorSet _set;
};

} // namespace bl