#pragma once

#include <string>
#include <vector>
#include <span>

#include "Field.h"
#include "Variant.h"

namespace bl
{

class Method
{
public:

    template<typename...args>
    Method();
    ~Method();

    void GetName();
    void Invoke();

};

class Field{};

class Object {

public:
    Object() = default;
    virtual ~Object() = default;

    Object* New();
    Object* Clone();

    std::string_view GetClassName();

    std::span<Method> GetMethods();
    std::span<Field> GetFields();

 
    void SetField(const std::string& name, Variant value);


};

class ObjectDB
{
    static ObjectDB* _instance;

    std::vector<>

public:
    static 
    static Object* New(std::string_view name);

};

} // namespace bl