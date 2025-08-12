#pragma once

#include "Graphics/VulkanImage.h"
#include "Resource.h"

namespace bl {

/**
 * @class Texture
 * Interface class for texture resources.
 */
class Texture : public Resource {

protected:
    VulkanDevice* _device;
    std::unique_ptr<VulkanImage> _image;

public:
    virtual const std::string& GetType() const = 0;

    Texture(ResourceManager* manager, VulkanDevice* device)
        : Resource(manager)
        , _device(device)
    {
    }

    virtual ~Texture()
    {
    }

    virtual bool Load() = 0;
    virtual void Unload() = 0;
    virtual bool ExportBinary(std::ostream& stream) const = 0;

    VulkanImage* GetImage() { return _image.get(); }
};

}