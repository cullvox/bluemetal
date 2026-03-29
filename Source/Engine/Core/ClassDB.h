#pragma once

#include <vector>
#include <string_view>
#include <span>

#include "Object.h"
#include "Reflection/Property.h"

namespace bl
{

using ObjectInstantiationFunc = Object* (* )(Engine& engine);

class ClassDB
{
    Engine& _engine;

    struct ClassData
    {
        std::string_view name;
        ObjectInstantiationFunc instantiationFunc;
        std::vector<std::unique_ptr<Property>> properties;
        std::vector<Property*> userPropertyPointers;
        std::unordered_map<std::string_view, std::size_t> nameToPropertyIndex;
    };

    std::vector<ClassData> _classes;
    std::unordered_map<std::string_view, std::size_t> _nameToClassIndex;

public:
    ClassDB(Engine& engine);
    ~ClassDB();

    Object* Instantiate(const std::string_view className);

    template<typename T>
    void Register()
    {
        T::RegisterClass(*this);
    }


    void RegisterClass(const std::string_view className, ObjectInstantiationFunc instantiationFunc);
    void RegisterProperty(const std::string_view className, std::unique_ptr<Property> property);

    bool HasClass(const std::string_view name);
    std::span<Property*> GetClassProperties(const std::string_view name);

};


} // namespace bl