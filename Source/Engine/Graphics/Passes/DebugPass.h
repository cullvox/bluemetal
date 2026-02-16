#pragma once

#include <glm/vec3.hpp>

#include <Core/Color.h>
#include "ColorPass.h"

namespace bl {

class DebugPass : public ColorPass {
public:
    DebugPass();
    ~DebugPass();

    void SetDebugMaterialInstance(Ref<MaterialInstance> instance);


private:

    struct DebugVertex {

    };

};

}