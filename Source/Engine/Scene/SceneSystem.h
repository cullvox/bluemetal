#pragma once

#include "Engine/System.h"

#include "Resources/Scene.h"

namespace bl {

class Engine;

class SceneSystem : public System {
public:
    SceneSystem(Engine& engine);
    virtual ~SceneSystem();

    virtual std::shared_ptr<Resource> ConstructResource(std::size_t typeHash, const std::filesystem::path& path) override;
    void SetSceneActive(Ref<Scene> scene);
};

}