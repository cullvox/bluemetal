#pragma once

#include "Node3D.h"

namespace bl {

class AudioListener3D : public Node3D {
public:
    AudioListener3D(Engine& engine);
    ~AudioListener3D();

    virtual void Update(float dt) override;

    static void RegisterClass(ClassDB& db);
};

} // namespace bl
