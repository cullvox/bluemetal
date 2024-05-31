#pragma once

#include "Resource/Resource.h"
#include "Pipeline.h"
#include "DescriptorSetCache.h"

namespace bl
{

class MaterialInstance;

enum class MaterialPass
{
    eGeometry, /* Uses geometry pass (unlit) */
    eLit, /* Uses the lighting pass (lit) */
};

struct Uniform : public BlockMemberReflection
{
    uint32_t binding;
};

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
    Material(Device* device, const nlohmann::json& data);
    ~Material();

    virtual void Load();
    virtual void Unload();

    Pipeline* GetPipeline() const;
private:
    Device* _device;
    std::unique_ptr<Pipeline> pipeline;
    std::unique_ptr<DescriptorSetCache> _descriptorSetCache;
    std::unordered_map<std::string, Uniform> _uniforms;
    std::string _vert, _frag;
};

} // namespace bl