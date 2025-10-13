#pragma once

#include "Precompiled.h"
#include "Concepts.h"

namespace bl {

class Object;

class ObjectClasses {
    static std::unordered_map<std::string_view, ObjectClass> _objects;

protected:
    static ObjectClass& AddObjectClass(std::string_view name);

public:
    static const ObjectClass* GetObjectClass(std::string_view name);
};

}