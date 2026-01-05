#pragma once

#include "Mesh.h"

namespace bl {

class SkinnedMesh : public Mesh {
public:
    SkinnedMesh(ResourceSystem& resourceSystem, GraphicsSystem* system, const std::filesystem::path& path);
    ~SkinnedMesh();
};

}