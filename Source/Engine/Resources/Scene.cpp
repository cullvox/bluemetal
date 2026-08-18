#include "Scene.h"
#include "Resources/Resource.h"

namespace bl
{

Scene::Scene()
    : Resource()
{

}

Scene::Scene(const std::filesystem::path& path)
    : Resource(path)
{
    // Load the scene from file.
    std::ifstream file(path);

    nlohmann::json json;
    file >> _data;
}

Scene::~Scene()
{

}

void Scene::Save()
{
}

void Scene::Release()
{
    Resource::Release();
    _data = {};
}

void Scene::Pack(Node* root)
{
}

std::unique_ptr<Node> Scene::Instantiate()
{
    std::unique_ptr<Node> root;

}

} // namespace bl