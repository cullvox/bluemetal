#include "ObjectClasses.h"

ObjectDefinition& ObjectClasses::AddObjectDefinition(std::string_view name)
{
    return _objects[name];
}

const ObjectDefinition* ObjectClasses::GetObjectDefinition(std::string_view name)
{
    return &_objects[name];
}

Object* ObjectClasses::CreateObject(std::string_view name)
{
    const ObjectDefinition* objectDef = GetObjectDefinition(name);
    if (!objectDef) return nullptr;
    if (!objectDef->creatorFunc) return nullptr;

    return objectDef->creatorFunc();
}