#pragma once

#include "Engine/System.h"

namespace bl {

class Engine;

class SceneSystem : public System {
public:
    SceneSystem(Engine& engine);
    ~SceneSystem();

    virtual std::shared_ptr<Resource> ConstructResource(ResourceSystem& resourceSystem, std::size_t typeHash, const std::filesystem::path& path) override;

    void LoadScene(Ref<Scene> scene);

};

}