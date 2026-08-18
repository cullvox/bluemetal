#pragma once

#include "Editor.h"

namespace bl {

class InspectorEditor : public Editor {
protected:
    void OnSelectedNodeChanged() override;

    void DrawProperties(Object* object, std::span<Property*> properties);
    void DrawObjectProperties(Object* object);


public:
    InspectorEditor();
    ~InspectorEditor();

    void Draw(bl::RenderData& rd) override;
};

}