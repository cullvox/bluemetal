#pragma once

#include <string_view>

#include "Object.h"

namespace bl
{


class ClassRegistration
{
    void AddMethod();
    void AddProperty();
};

class ClassDB
{

    Object* Instantiate(std::string_view className);
    std::string_view GetParentClassName(std::string_view className);

    void RegisterClass(std::string_view className);
    void RegisterProperty(Property& property);

};


} // namespace bl