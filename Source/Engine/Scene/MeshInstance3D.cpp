#include "MeshInstance3D.h"
#include "Engine/Engine.h"
#include "Resources/Material.h"
#include "Graphics/Renderer.h"
#include "Resources/ResourceSystem.h"

namespace bl {

MeshInstance3D::MeshInstance3D(Engine& engine)
    : Node3D(engine)
{
    _material = GetEngine().GetResourceSystem()->Load<bl::Material>("Resources/Materials/Default.mat");
}

MeshInstance3D::MeshInstance3D(const MeshInstance3D& node)
    : Node3D(node)
    , _material(node._material)
    , _mesh(node._mesh)
{
}

MeshInstance3D* MeshInstance3D::Clone()
{
    return new MeshInstance3D(*this);
}

void MeshInstance3D::Draw(RenderData& rd)
{
    // TODO: This isn't really instancing, the renderer will have to buffer instances.
    bl::InstanceData object {};
    object.model = GetWorldTransform();
    object.position = GetWorldPosition();

    rd.DrawInstance(_material.lock().get(), _mesh.lock().get(), object);

    //rd.renderer->AddInstance(_mesh.get(), object);

    Node3D::Draw(rd);
}

void MeshInstance3D::SetMesh(Ref<Mesh> mesh)
{
    _mesh = mesh;
}

void MeshInstance3D::SetMaterial(Ref<MaterialInstance> material)
{
    _material = material;
}

}