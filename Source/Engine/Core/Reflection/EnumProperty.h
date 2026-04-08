#pragma once

#include "Property.h"

namespace bl
{

template<typename T>
concept EnumType = requires { std::is_same_v<std::underlying_type_t<T>, int64_t>; };

template<typename TClass, EnumType TEnum>
class TEnumProperty : public Property
{
    using SetterType = void (TClass::*)(TEnum);
    using GetterType = TEnum (TClass::*)();

    SetterType _setter;
    GetterType _getter;
    std::string_view _type;

public:
    TEnumProperty(ClassDB& db, std::string_view enumType, std::string_view name, SetterType setter, GetterType getter)
        : Property(db, name, GetVariantType<int64_t>())
        , _type(enumType)
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TEnumProperty()
    {
    }

    void Set(Object* object, const Variant& value) override
    {
        if (value.index() != VariantTypeIndex<Variant, int64_t>())
        {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return;
        }

        // Check the value and make sure it's valid.
        int64_t enumValue = std::get<int64_t>(value);
        if (!GetClassDB().IsEnumValid(_type, enumValue))
            throw std::runtime_error("Invalid enum type!");

        (static_cast<TClass*>(object)->*_setter)(static_cast<TEnum>(enumValue));
    }

    Variant Get(Object* object)
    {
        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        return static_cast<int64_t>((static_cast<TClass*>(object)->*_getter)());
    }


};

} // namespace bl