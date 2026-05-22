#pragma once

#include "Property.h"

namespace bl {

template<typename TClass>
class TNamedProperty : public Property {

    Variant (TClass::* getter)(std::string_view propertyName);
    void (TClass::* setter)(std::string_view propertyName, const Variant& value);

public:
    TNamedProperty(ClassDB& db, const std::string_view name, VariantType type, PropertyFlags flags, void (TClass::* setter)(std::string_view propertyName, const Variant& value), Variant (TClass::* getter)(std::string_view propertyName))
        : Property(db, name, flags, type)
        , setter(setter)
        , getter(getter)
    {
    }

    ~TNamedProperty()
    {
    }

    virtual void Set(Object* object, Variant value) override
    {
        if (value.index() != static_cast<std::size_t>(GetType()))
        {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        // Perform normalization if the flag is set and the type supports it.
        if (HasFlag(PropertyFlags::Normalize)) {
            std::visit([&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<T, glm::quat> ||
                              std::is_same_v<T, glm::vec2> ||
                              std::is_same_v<T, glm::vec3> ||
                              std::is_same_v<T, glm::vec4>) {
                    value = glm::normalize(arg);
                } else {
                    Print::Error("Property ({}) has Normalize flag but does not support normalization.", GetName());
                }
            }, value);
        }

        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return;
        }

        (static_cast<TClass*>(object)->*setter)(GetName(), value);
    }

    virtual Variant Get(Object* object) override
    {
        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        return (static_cast<TClass*>(object)->*getter)(GetName());
    }
};

} // namespace bl
