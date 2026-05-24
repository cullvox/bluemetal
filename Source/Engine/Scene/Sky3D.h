#pragma once

#include "Node3D.h"
#include "Resources/MaterialInstance.h"

namespace bl {

class Sky3D : public Node3D {
    OBJECT_BOILER(Sky3D, Node3D)

    MaterialInstance* material;

public:
    Sky3D(Engine& engine);
    ~Sky3D();

    static void RegisterClass(ClassDB& db);

    virtual void Update(float deltaTime) override;
    virtual void Draw(RenderData& rd) override;

    void SetSkyMaterial(MaterialInstance* material);
    MaterialInstance* GetSkyMaterial();
};


} // namespace bl