#include "MeshInstance3D.h"
#include "Engine/Engine.h"
#include "Resources/Material.h"
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