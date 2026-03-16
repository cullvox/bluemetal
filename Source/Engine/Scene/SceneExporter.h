#pragma once

#include "Node.h"

namespace bl
{

class SceneExporter
{
public:
    SceneExporter();
    ~SceneExporter();

    void Export(Node* rootNode);

};

} // namespace bl