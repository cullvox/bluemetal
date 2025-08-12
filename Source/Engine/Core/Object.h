#pragma once

#include "Precompiled.h"

class Object
{
    struct ObjectDescriptor
    {
        Object* (*_builder)();
    };

    static std::unordered_map<std::string, ObjectDescriptor> _descriptors;

public:
    std::vector<Parameter*> GetParameters();
};

#define CLASS_OBJECT(Name) \
    static 