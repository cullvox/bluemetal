#pragma once

#include <string_view>


#define OBJECT_BOILER(name, parent) \
public: \
    static std::string_view GetClassName() { return BL_STRINGIFY(name); } \
    static std::string_view GetParentClassName() { return BL_STRINGIFY(parent); } \
private:

namespace bl
{

class Object {
public:
    Object() = default;
    Object(Object& rhs) = default;
    virtual ~Object() = default;

    static std::string_view GetClassName() { return "Object"; }

    virtual std::string_view ClassName();
    virtual std::string_view ParentClassName();
    virtual Object* Clone();

    static void RegisterClass()
    {
        
    }
};

} // namespace bl