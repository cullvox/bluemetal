#include "SceneSystem.h"

namespace bl {

SceneSystem::SceneSystem()
    : System()
{
}

SceneSystem::~SceneSystem()
{
}

SceneSystem* SceneSystem::Get()
{
    static SceneSystem system;
    return &system;
}

void SceneSystem::SetSceneActive(Ref<Scene> scene)
{

}

}