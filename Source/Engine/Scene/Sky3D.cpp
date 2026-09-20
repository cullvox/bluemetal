#include "Sky3D.h"
#include "Engine/Engine.h"
#include "Core/Reflection/TProperty.h"
#include "Core/ClassDB.h"
#include "Graphics/RenderData.h"

namespace bl {

Sky3D::Sky3D()
    : Node3D()
    , material(nullptr)
{
}

Sky3D::~Sky3D()
{
}

void Sky3D::RegisterClass()
{
    auto db = ClassDB::Get();
    db->RegisterClass("Sky3D", std::string_view("Node3D"), &Sky3D::Create);
    db->RegisterProperty("Sky3D", MakeClone<TProperty<Sky3D, Ref<MaterialInstance>>>("Sky Material", PropertyFlags::Editor, &Sky3D::SetSkyMaterial, &Sky3D::GetSkyMaterial));
}

void Sky3D::Draw(RenderData& rd)
{
    rd.Draw(material ? material->GetInstance() : nullptr, 6);
}

void Sky3D::Update(float deltaTime)
{
    Node3D::Update(deltaTime);

    //auto normalizedRotation = glm::normalize(GetRotationEuler());

    material->SetVector3("material.sunDirection",  glm::normalize(GetRotation() * glm::vec3(0.0f, 0.0f, -1.0f)));
}

void Sky3D::SetSkyMaterial(Ref<MaterialInstance> material)
{
    this->material = material;
}

Ref<MaterialInstance> Sky3D::GetSkyMaterial()
{
    return material;
}

} // namespace bl