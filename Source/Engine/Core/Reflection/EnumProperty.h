#pragma once

#include "Core/Variant.h"
#include "Property.h"
#include <type_traits>

namespace bl
{

template<typename T>
concept EnumType = std::is_enum_v<T> && std::is_same_v<std::underlying_type_t<T>, int64_t>;

/// - Enum properties are enumerable properties which are a mapping of integers
/// and strings.
template<typename TClass, EnumType TEnum>
class TEnumProperty : public Property
{
    using SetterType = void (TClass::*)(TEnum);
    using GetterType = TEnum (TClass::*)();

    SetterType _setter;
    GetterType _getter;
    std::string_view _type;

public:

    TEnumProperty(ClassDB& db, std::string_view enumType, std::string_view propertyName, PropertyFlags flags, SetterType setter, GetterType getter)
        : Property(db, propertyName, flags, GetVariantType<int64_t>())
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
        // Ensure that the variant is the correct type.
        if (value.index() != VariantTypeIndex<Variant, EnumValue>()) {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        // Ensure that the object is the correct type for this property.
        if (dynamic_cast<TClass*>(object) == nullptr) {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return;
        }

        // Ensure the enum value is a valid value.
        auto enumValue = std::get<EnumValue>(value);
        if (!GetClassDB().IsEnumValid(_type, enumValue.value)) {
            Print::Error("Invalid enum value ({}) to set on object ({}) with the specified enum type ({}).", enumValue.value, TClass::GetStaticClassName(), enumValue.enumName);
            return;
        }

        // Call setter.
        (static_cast<TClass*>(object)->*_setter)(static_cast<TEnum>(enumValue.value));
    }

    Variant Get(Object* object) override
    {
        // Ensure the object is the correct type for this property.
        if (dynamic_cast<TClass*>(object) == nullptr) {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        // Call getter.
        return EnumValue{_type, static_cast<int64_t>((static_cast<TClass*>(object)->*_getter)())};
    }
};

} // namespace bl