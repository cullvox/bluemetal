#pragma once

#include "Material.h"
#include "Resource.h"

namespace bl {

class MaterialInstance : public Resource {
    OBJECT_CLASS(MaterialInstance, Resource)

    Ref<Material> material;
protected:
    virtual bool Load();
    virtual void Unload();
    virtual bool ExportBinary(std::ostream& stream) const;

public:
    MaterialInstance();
    ~MaterialInstance();

};

}