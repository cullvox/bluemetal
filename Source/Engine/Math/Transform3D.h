#pragma once

#include "Math/Math.h"

namespace bl {

class Transform3D {



public:
    Transform3D();
    Transform3D(const glm::mat4& matrix);
    Transform3D(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale);
    Transform3D(const glm::vec3& position, const glm::vec3& eulerAngleDegrees, const glm::vec3& scale);
    ~Transform3D();

    void SetParent(Transform3D* parent);
    void SetPosition(const glm::vec3& position);
    void SetRotation(const glm::vec3& eulerAngleDegrees);
    void SetRotation(const glm::quat& rotation);
    void SetScale(const glm::vec3& scale);
    void SetWorldPosition(const glm::vec3& position);
    void SetWorldRotation(const glm::quat& rotation);
    void SetWorldRotation(const glm::vec3& eulerAngleDegrees);
    void SetWorldScale(const glm::vec3& scale);

    Transform3D* GetParent() const;
    const glm::vec3& GetPosition() const;
    const glm::vec3& GetRotation() const;
    const glm::quat& GetRotationQuat() const;
    const glm::vec3& GetScale() const;
    const glm::mat4& GetMatrix();
    const glm::vec3& GetWorldPosition();
    const glm::vec3& GetWorldRotation();
    const glm::quat& GetWorldRotationQuat();
    const glm::vec3& GetWorldScale();
    const glm::mat4& GetWorldMatrix();
};

}