#pragma once

#include "Entity.h"
#include "Math/Transform.h"

namespace bl
{

/** @brief An actor is an entity that physically exists in the world. */
class Actor : public Entity
{
public:
    Actor(
        const std::string& name,
        uint64_t uid);
    ~Actor();

    Transform& GetTransform();

protected:
    Transform transform;
};

} // namespace bl