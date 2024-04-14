#pragma once

#include "renderdoc_app.h"

namespace bl
{

class RenderDoc 
{
public:
    RenderDoc();
    ~RenderDoc();

    void beginCapture();
    void endCapture();
    bool isCapturing() const;

private:
    bool    _isCapturing;
    RENDERDOC_API_1_1_2 *rdoc_api = nullptr;
};

} // namespace bl