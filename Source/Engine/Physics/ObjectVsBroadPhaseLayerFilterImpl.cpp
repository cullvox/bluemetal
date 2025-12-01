#include "ObjectVsBroadPhaseLayerFilterImpl.h"
#include "BroadPhaseLayers.h"
#include "ObjectLayers.h"

namespace bl {

bool ObjectVsBroadPhaseLayerFilterImpl::ShouldCollide(JPH::ObjectLayer inObject1, JPH::BroadPhaseLayer inLayer2) const
{
    // Define collision rules between object layers and broad phase layers
    switch (inObject1)
    {
    case ObjectLayers::STATIC:
        return inLayer2 == BroadPhaseLayers::MOVING;
    case ObjectLayers::MOVABLE:
        return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING;
    case ObjectLayers::TRIGGER:
        return inLayer2 == BroadPhaseLayers::MOVING;
    case ObjectLayers::PROJECTILE:
        return inLayer2 == BroadPhaseLayers::NON_MOVING || inLayer2 == BroadPhaseLayers::MOVING;
    default:
        return false;
    }
}

} // namespace bl