#include "MeshInstance3D.h"
#include "Engine/Engine.h"
#include "Resources/Material.h"

namespace bl
{

MeshInstance3D::MeshInstance3D(Engine* engine)
    : Node3D(engine)
{
    _material = engine->GetResourceManager()->Load<bl::Material>("Resources/Materials/Default.mat");
}

void MeshInstance3D::Draw(VulkanRenderData& rd)
{
    // TODO: This isn't really instancing, the renderer will have to buffer instances.
    _material->Bind(rd);

    bl::ObjectPC object{};
    object.model = glm::identity<glm::mat4>();
    object.model = glm::translate(object.model, glm::vec3{0.0f, 0.0f, -30.0f});
    object.model = glm::scale(object.model, glm::vec3{1/100.0f, 1/100.0f, 1/100.0f});

    _material->PushConstant(rd, 0, sizeof(ObjectPC), &object);

    _mesh->Bind(rd.cmd);
    vkCmdDrawIndexed(rd.cmd, _mesh->GetIndicesCount(), 1, 0, 0, 0);

    Node3D::Draw(rd);
}

void MeshInstance3D::SetMesh(Ref<Mesh> mesh)
{
    _mesh = mesh;
}

}