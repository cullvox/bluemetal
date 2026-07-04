#include "AABB.h"

namespace bl
{

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
    : _min(min)
    , _max(max)
{
}

const glm::vec3& AABB::GetMin() const
{
    return _min;
}

const glm::vec3& AABB::GetMax() const
{
    return _max;
}

glm::vec3 AABB::GetCenter() const
{
    return (_min + _max) * 0.5f;
}

glm::vec3 AABB::GetSize() const
{
    return _max - _min;
}

}
