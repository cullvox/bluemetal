#include "MeshInstance3D.h"
#include "Engine/Engine.h"
#include "Resources/Material.h"
#include "Graphics/Renderer.h"
#include "Resources/ResourceSystem.h"
#include "Core/Profiler.h"
#include "Core/ClassDB.h"

namespace bl {

MeshInstance3D::MeshInstance3D()
    : Node3D()
{
    _material = ResourceSystem::Get()->Load<bl::Material>("Resources/Materials/Default.json");
}

MeshInstance3D::MeshInstance3D(const MeshInstance3D& node)
    : Node3D(node)
    , _material(node._material)
    , _mesh(node._mesh)
{
}

void MeshInstance3D::Draw(RenderData& rd)
{
    // TODO: This isn't really instancing, the renderer will have to buffer instances.
    //bl::InstanceData object {};
    //object.model = GetWorldMatrix();

    rd.DrawInstance(this, _material.lock().get()->GetInstance(), _mesh.lock().get()->GetMesh(), GetWorldMatrix());

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

void MeshInstance3D::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("MeshInstance3D", "Node3D", &MeshInstance3D::Create);
    //db.RegisterProperty("MeshInstance3D", std::make_unique<TProperty<MeshInstance3D, ResourceReference<Mesh>>>("mesh", ))

}

}