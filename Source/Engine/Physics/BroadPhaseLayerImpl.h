#pragma once

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

#include "BroadPhaseLayers.h"
#include "ObjectLayers.h"

namespace bl {

class BroadPhaseLayerImpl : public JPH::BroadPhaseLayerInterface {
    std::array<JPH::BroadPhaseLayer, ObjectLayers::NUM_LAYERS> _objectToBroadPhaseLayer;
public:
    BroadPhaseLayerImpl();

    virtual uint32_t GetNumBroadPhaseLayers() const override;
    virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer layer) const override;

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
    virtual const char* GetBroadPhaseLayerName(JPH::BroadPhaseLayer layer) const override;
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

};

} // namespace bl