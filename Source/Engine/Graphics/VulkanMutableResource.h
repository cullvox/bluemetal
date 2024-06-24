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
class VulkanMutableResource {
public:
    VulkanMutableResource() = default;
    ~VulkanMutableResource() = default;

    /// @brief Returns the hash of the mutable resource.
    ///
    /// In reality when implemented this function should do almost nothing
    /// except return a std::size_t value when called. This way we don't have
    /// to hash the create info every call.
    virtual std::size_t GetHash() const = 0;
};

template<class T>
struct VulkanMutableResourceReference {
    static_assert(std::is_base_of_v<VulkanMutableResource, T> == true && "T must be based on VulkanMutableResource!");
    
    VulkanMutableResourceReference();
    VulkanMutableResourceReference(const VulkanMutableResourceReference& rhs) = default;
    VulkanMutableResourceReference(VulkanMutableResourceReference&& rhs) = default;
    VulkanMutableResourceReference(T* resource) 
        : _resource(resource) {} // Allow a call to WasChanged() to update the _currentHash.
    ~VulkanMutableResourceReference() = default;

    VulkanMutableResourceReference& operator=(const VulkanMutableResourceReference& rhs) = default;
    VulkanMutableResourceReference& operator=(VulkanMutableResourceReference&& rhs) = default;
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