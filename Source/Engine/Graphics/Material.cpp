#include "Material.h"

namespace bl
{

Material::Material(Device* device, const nlohmann::json& data)
    : Resource(data)
    , _device(device)
{
}

void Material::Load()
{
}

void Material::Unload()
{
}



} // namespace bl
