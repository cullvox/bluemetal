#pragma once

namespace bl 
{

/// AABB (Axis-Aligned Bounding Box)
class AABB
{
    glm::vec3 _min;
    glm::vec3 _max;

public:
    AABB() = default;
    AABB(const glm::vec3& min, const glm::vec3& max);

    const glm::vec3& GetMin() const;
    const glm::vec3& GetMax() const;

    glm::vec3 GetCenter() const;
    glm::vec3 GetSize() const;
};

} // namespace bl