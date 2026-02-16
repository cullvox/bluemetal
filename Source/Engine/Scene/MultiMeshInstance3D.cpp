#include "MultiMeshInstance3D.h"
#include "Graphics/RenderData.h"

namespace bl {

MultiMeshInstance3D::MultiMeshInstance3D(Engine& engine)
    : Node3D(engine)
    , _mesh({})
    , _instanceCount(0)
{
}

MultiMeshInstance3D::MultiMeshInstance3D(const MultiMeshInstance3D& other)
    : Node3D(other)
    , _mesh(other._mesh)
    , _instanceCount(other._instanceCount)
    , _instanceTransforms(other._instanceTransforms)
{
}

MultiMeshInstance3D::~MultiMeshInstance3D()
{
}

MultiMeshInstance3D* MultiMeshInstance3D::Clone()
{
    return new MultiMeshInstance3D(*this);
}

void MultiMeshInstance3D::Draw(RenderData& rd)
{
    auto meshPtr = _mesh.lock();
    auto materialPtr = _material.lock();

    if (meshPtr && materialPtr && _instanceCount > 0 && !_instanceTransforms.empty()) {

        for (uint32_t i = 0; i < _instanceCount; ++i) {
            InstanceData instanceData;
            instanceData.model = _instanceTransforms[i];
            instanceData.position = glm::vec4(_instanceTransforms[i][3]);
            rd.DrawInstance(this, materialPtr.get(), meshPtr.get(), instanceData);
        }
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

}