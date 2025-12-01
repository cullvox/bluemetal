#include "BroadPhaseLayerImpl.h"
#include "ObjectLayers.h"

namespace bl {

BroadPhaseLayerImpl::BroadPhaseLayerImpl()
{
    _objectToBroadPhaseLayer[ObjectLayers::STATIC] = BroadPhaseLayers::NON_MOVING;
    _objectToBroadPhaseLayer[ObjectLayers::MOVABLE] = BroadPhaseLayers::MOVING;
    _objectToBroadPhaseLayer[ObjectLayers::TRIGGER] = BroadPhaseLayers::TRIGGER;
    _objectToBroadPhaseLayer[ObjectLayers::PROJECTILE] = BroadPhaseLayers::MOVING;
}

uint32_t BroadPhaseLayerImpl::GetNumBroadPhaseLayers() const
{
    return BroadPhaseLayers::NUM_LAYERS;
}

JPH::BroadPhaseLayer BroadPhaseLayerImpl::GetBroadPhaseLayer(JPH::ObjectLayer layer) const
{
    return _objectToBroadPhaseLayer[layer];
}

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
const char* BroadPhaseLayerImpl::GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const
{
    switch (static_cast<uint8_t>(layer))
    {
    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::NON_MOVING):
        return "NON_MOVING";
    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::MOVING):
        return "MOVING";
    case static_cast<JPH::BroadPhaseLayer::Type>(BroadPhaseLayers::TRIGGER):
        return "TRIGGER";
    default:
        return "UNKNOWN";
    }
}
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

} // namespace bl
