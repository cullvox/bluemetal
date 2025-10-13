#pragma once

#include "Precompiled.h"
#include "Concepts.h"
#include "ObjectMacros.h"

#include "Property.h"

namespace bl {

class Object {
    static Object* _Creator();
    static void BindProperties();

public:
    virtual ~Object() = default;

    // STATIC FUNCTIONS DEFINED BY OBJECT_CLASS(NAME, PARENT) MACRO
    // static const ObjectClass* GetClass();
    // static const ObjectClass* GetParentClass();

    // STATIC FUNCTIONS DEFINED BY USER
    // static void BindProperties()
    // {
    //     _properties.emplace_back(Property<float>("foo", PropertyFlagBits::eDefault, &Object::SetFoo, &Object::GetFoo));
    // }

    template<typename T>
    T GetPropertyValue(std::string_view name);

    template<typename T>
    void SetPropertyValue(std::string_view name, const T& value);
};

}