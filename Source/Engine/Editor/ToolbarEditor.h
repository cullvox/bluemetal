#pragma once

#include "Editor.h"
#include "Graphics/RenderData.h"

namespace bl {

enum class TransformMode {
    eTranslate,
    eRotate,
    eScale,
};

class ToolbarEditor : public Editor {

    TransformMode _transformMode;

public:
    ToolbarEditor();
    ~ToolbarEditor();

    void SetTransformMode(TransformMode mode);

    virtual void Draw(RenderData& rd);
};

}