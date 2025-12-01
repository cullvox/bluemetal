#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace bl {

namespace ObjectLayers {

    /** @brief Static objects are object that will not move.  */
    static constexpr JPH::ObjectLayer STATIC = 0;

    /** @brief Movable objects are objects that can move and interact with static objects. */
    static constexpr JPH::ObjectLayer MOVABLE = 1;

    /** @brief Trigger objects are objects that do not collide but can generate trigger events. */
    static constexpr JPH::ObjectLayer TRIGGER = 2;

    /** @brief Projectiles are fast moving objects that can interact with both static and movable objects. */
    static constexpr JPH::ObjectLayer PROJECTILE = 3;

    static constexpr uint8_t NUM_LAYERS = 4;
}

}