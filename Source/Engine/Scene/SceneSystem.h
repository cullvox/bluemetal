#pragma once

#include "Engine/System.h"

#include "Resources/Scene.h"

namespace bl {

class Engine;

class SceneSystem : public System {
    SceneSystem();
    virtual ~SceneSystem();
public:
    static SceneSystem* Get();

    void SetSceneActive(Ref<Scene> scene);
};

}