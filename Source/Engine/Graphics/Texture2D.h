#pragma once

#include "Image.h"

namespace bl {

class Texture2D : public Resource { 
public:
    Texture2D(const nlohmann::json& data);
    ~Texture2D();

    virtual void Load();
    virtual void Unload();

private:
    Image _image;
};

} // namespace bl