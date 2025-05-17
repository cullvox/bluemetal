#pragma once

#include <any>

namespace bl
{

enum class PropertyType
{
    eInteger,
    eBoolean,
    eFloat,
    eDouble,
    eVector2,
    eVector3,
    eVector4,
    eMatrix4,
};

class Property
{
public:
    Property(const std::string& name, std::any value);

    void SetValue(std::any value);
    std::any GetValue();

private:
    std::string _name;
    std::any _ref;
};

}