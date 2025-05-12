#pragma once

#include "Precompiled.h"

namespace bl {

/// @brief A resource that's hashed so we know if it's data was changed.
///
/// Some resources are immutable like textures but, for some it makes more
/// sense to be mutable, like samplers. To allow for a sampler to be updated
/// and have a dependant resource know it's been changed we hash it's creation
/// data and those dependants can check if the data was changed to update
/// themselves accordingly.
///
/// Using a VulkanMutableReference we can determine if the mutable has changed
/// and support the updatable objects we want.
class VulkanMutable {
public:
    VulkanMutable() = default;
    ~VulkanMutable() = default;

    /// @brief Returns the hash of the mutable resource.
    ///
    /// In reality when implemented this function should do almost nothing
    /// except return a std::size_t value when called. This way we don't have
    /// to hash the create info every call.
    virtual std::size_t GetHash() const = 0;
};

/// @brief Stores the previously known hash of the referenced object.
///
/// You can check if a Vulkan object has changed by checking if create info
/// is the same or different as what it used to be. This way we can support
/// resources that are recreated in real time.
template<class T>
struct VulkanMutableReference {
    static_assert(std::is_base_of_v<VulkanMutable, T> == true && "T must be based on VulkanMutableResource!");
    
    VulkanMutableReference();
    VulkanMutableReference(const VulkanMutableReference& rhs) = default;
    VulkanMutableReference(VulkanMutableReference&& rhs) = default;
    VulkanMutableReference(T* resource) 
        : _resource(resource) {} // Allow a call to WasChanged() to update the _currentHash.
    ~VulkanMutableReference() = default;

    VulkanMutableReference& operator=(const VulkanMutableReference& rhs) = default;
    VulkanMutableReference& operator=(VulkanMutableReference&& rhs) = default;
    T* operator->() const { 
        assert(_resource != nullptr && "Resource must not be NULL!");  
        return _resource; 
    }
    
    bool WasChanged() {
        assert(_resource != nullptr && "Resource must not be NULL!");

        auto hash = _resource->GetHash();
        if (_currentHash == hash) return false;

        _currentHash = hash;
        return true;
    }

private:
    T* _resource;
    std::size_t _currentHash;
};

} // namespace bl