#include "ObjectLayerPairFilterImpl.h"
#include "ObjectLayers.h"

namespace bl {

bool ObjectLayerPairFilterImpl::ShouldCollide(JPH::ObjectLayer inLayer1, JPH::ObjectLayer inLayer2) const
{
    // Define collision rules between object layers
    if (inLayer1 == ObjectLayers::TRIGGER || inLayer2 == ObjectLayers::TRIGGER)
    {
        // Triggers do not collide with anything
        return false;
    }

    // Static objects do not collide with other static objects
    if (inLayer1 == ObjectLayers::STATIC && inLayer2 == ObjectLayers::STATIC)
    {
        return false;
    }

    // All other combinations collide
    return true;
}

} // namespace bl