#include "MeshInstance3D.h"

namespace bl
{

MeshInstance3D::MeshInstance3D(Engine* engine)
    : Node3D(engine)
{
}

void MeshInstance3D::Draw(VulkanRenderData& rd)
{
    // TODO: This isn't really instancing, the renderer will have to buffer instances.
    _mesh->Bind(rd.cmd);
    vkCmdDrawIndexed(rd.cmd, 0, 1, 0, 0, 0);
}

void MeshInstance3D::SetMesh(Ref<Mesh> mesh)
{
    _mesh = mesh;
}

}