#include "Material.h"

namespace bl
{

Material::Material(Device* device, const nlohmann::json& data)
    : Resource(data)
    , _device(device)
{
    try 
    {
        _vert = data["shaders"]["vert"].get<std::string>();
        _frag = data["shaders"]["frag"].get<std::string>();
    }
    catch ()
    {
        
    }



}

void Material::Load()
{
    ResourceRef<Shader> _vertShader;
    ResourceRef<Shader> _fragShader;

    PipelineCreateInfo info = {};
    info.

    _pipeline = std::make_unique<Pipeline>(_device, );
}

void Material::Unload()
{
}



} // namespace bl
