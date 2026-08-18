#pragma once

#include "Node3D.h"
#include "Resources/Mesh.h"
#include "Resources/MaterialInstance.h"

namespace bl {

/// @brief A node that can render multiple instances of a mesh with different transforms.
/// This is more efficient than creating multiple MeshInstance3D nodes when rendering many copies of the same mesh.
class MultiMeshInstance3D : public Node3D {
    OBJECT_BOILER(MultiMeshInstance3D, Node3D)

    Ref<Mesh> _mesh;
    Ref<MaterialInstance> _material;
    uint32_t _instanceCount;
    std::vector<glm::mat4> _instanceTransforms;
public:
    MultiMeshInstance3D();
    MultiMeshInstance3D(const MultiMeshInstance3D& rhs);
    ~MultiMeshInstance3D();

    /// @brief Draws the multi-mesh instance.
    /// @param rd Render data for the current frame.
    virtual void Draw(RenderData& rd) override;

    /// @brief Sets the mesh to be instanced.
    /// @param mesh Reference to the mesh resource.
    void SetMesh(Ref<Mesh> mesh);

    /// @brief Gets the mesh being instanced.
    /// @return Reference to the mesh resource.
    Ref<Mesh> GetMesh();

    /// @brief Sets the material for all instances.
    /// @param material Reference to the material instance.
    void SetMaterial(Ref<MaterialInstance> material);

    /// @brief Gets the material for all instances.
    /// @return Reference to the material instance.
    Ref<MaterialInstance> GetMaterial();

    /// @brief Sets the number of instances to render.
    /// @param count Number of instances.
    void SetInstanceCount(uint32_t count);

    /// @brief Gets the number of instances to render.
    /// @return Number of instances.
    uint32_t GetInstanceCount();

    /// @brief Sets the transform for a specific instance.
    /// @param index Index of the instance.
    /// @param transform Transform matrix for the instance.
    void SetInstanceTransform(uint32_t index, const glm::mat4& transform);

    static void RegisterClass();
};

} // namespace bl