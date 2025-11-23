#include "MeshInstance3D.h"
#include "Engine/Engine.h"
#include "Resources/Material.h"

namespace bl {

MeshInstance3D::MeshInstance3D(Engine* engine)
    : Node3D(engine)
{
    _material = engine->GetResourceManager()->Load<bl::Material>("Resources/Materials/Default.mat");
}

void MeshInstance3D::Draw(VulkanRenderData& rd)
{
    // TODO: This isn't really instancing, the renderer will have to buffer instances.
    bl::ObjectPC object {};
    object.model = GetWorldTransform();

    _material.lock()->Bind(rd);
    _mesh.lock()->Bind(rd.cmd);
    _material.lock()->PushConstant(rd, 0, sizeof(ObjectPC), &object);

    vkCmdDrawIndexed(rd.cmd, _mesh.lock()->GetIndicesCount(), 1, 0, 0, 0);

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