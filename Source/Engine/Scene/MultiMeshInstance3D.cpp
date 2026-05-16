#include "MultiMeshInstance3D.h"
#include "Graphics/RenderData.h"
#include "Core/ClassDB.h"

namespace bl {

MultiMeshInstance3D::MultiMeshInstance3D(Engine& engine)
    : Node3D(engine)
    , _mesh({})
    , _instanceCount(0)
{
}

MultiMeshInstance3D::MultiMeshInstance3D(const MultiMeshInstance3D& rhs)
    : Node3D(rhs)
    , _mesh(rhs._mesh)
    , _instanceCount(rhs._instanceCount)
    , _instanceTransforms(rhs._instanceTransforms)
{
}

MultiMeshInstance3D::~MultiMeshInstance3D()
{
}

void MultiMeshInstance3D::Draw(RenderData& rd)
{
    auto meshPtr = _mesh.lock();
    auto materialPtr = _material.lock();

    if (meshPtr && materialPtr && _instanceCount > 0 && !_instanceTransforms.empty()) {
        rd.DrawMultiInstance(this, materialPtr.get()->GetInstance(), meshPtr.get()->GetMesh(), _instanceTransforms);
    }
}

void MultiMeshInstance3D::SetMesh(Ref<Mesh> mesh)
{
    _mesh = mesh;
}

Ref<Mesh> MultiMeshInstance3D::GetMesh()
{
    return _mesh;
}

void MultiMeshInstance3D::SetMaterial(Ref<MaterialInstance> material)
{
    _material = material;
}

Ref<MaterialInstance> MultiMeshInstance3D::GetMaterial()
{
    return _material;
}

void MultiMeshInstance3D::SetInstanceCount(uint32_t count)
{
    _instanceCount = count;
    _instanceTransforms.resize(count);
}

uint32_t MultiMeshInstance3D::GetInstanceCount()
{
    return _instanceCount;
}

void MultiMeshInstance3D::SetInstanceTransform(uint32_t index, const glm::mat4& transform)
{
    if (index < _instanceTransforms.size()) {
        _instanceTransforms[index] = transform;
    }
}

void MultiMeshInstance3D::RegisterClass(ClassDB& db)
{
    db.RegisterClass("MultiMeshInstance3D", "Node3D", &MultiMeshInstance3D::Create);
}

}