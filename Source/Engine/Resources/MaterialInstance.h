#pragma once

#include "Material.h"
#include "Resource/Resource.h"

namespace bl {

class MaterialInstance : public Resource {
    Ref<Material> material;
protected:
    virtual const std::string& GetType() const override { return "MaterialInstance"; }
    virtual void Load();
    virtual void Unload();
    virtual void ExportBinary(std::ostream& stream) const;

public:
    MaterialInstance(ResourceManager* manager, const nlohmann::json& data);
    ~MaterialInstance();

};

}