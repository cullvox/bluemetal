#pragma once

#include "Node3D.h"

class AudioListener3D : public Node3D
{
public:
    AudioListener3D();
    ~AudioListener3D();

    virtual void Update(float dt) override;
};