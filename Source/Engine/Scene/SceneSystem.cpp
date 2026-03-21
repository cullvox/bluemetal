#include "SceneSystem.h"

namespace bl {

SceneSystem::SceneSystem(Engine& engine)
    : System(engine)
{
}

SceneSystem::~SceneSystem()
{
}

std::shared_ptr<Resource> SceneSystem::ConstructResource(std::size_t typeHash, const std::filesystem::path& path)
{
    throw std::runtime_error("No scene loading made");
}

void SceneSystem::SetSceneActive(Ref<Scene> scene)
{

}

}