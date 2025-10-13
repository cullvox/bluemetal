#pragma once

#include "Precompiled.h"
#include "Flags.h"

namespace bl {

class Object;

template<typename T>
concept SerializableType = std::default_initializable<T> &&
    (std::is_trivial_v<T> ||
        requires (T p, std::istream& i, std::ostream& o) {
            { p.Serialize(i) } -> std::same_as<void>;
            { p.Deserialize(o) } -> std::same_as<void>;
        }
    );

template<typename T>
concept PropertyType = requires {SerializableType<T>;};

class Property {

    std::string_view _name;
public:
    Property(std::string_view name)
        : _name(name)
    {
    }

    std::string_view GetName()
    {
        return _name;
    }
};

enum class PropertyFlagBits : uint32_t
{
    eReadOnly,
};
using PropertyFlags = Flags<PropertyFlagBits>;
BL_DEFINE_FLAG_OPS(PropertyFlagBits)

template <ObjectType TObject, PropertyType TProp>
class PropertyValue : public Property {
public:
    using Getter = void (TObject::*)(TProp);
    using Setter = TProp (TObject::*)(void);

private:
    std::string_view _name;
    PropertyFlags _flags;
    Getter _getter;
    Setter _setter;

public:
    PropertyValue(std::string_view name, Getter getter, Setter setter)
        : _name(name)
        , _getter(getter)
        , _setter(setter)
    {
    }

    virtual std::any Get(const Object* object) override
    {
        TObject* obj = dynamic_cast<const TObject*>(object);
        assert(obj); // This should never fail.
        return std::invoke(_getter, obj);
    }

    virtual void Set(const Object* object, std::any value) override
    {
        TObject* obj = dynamic_cast<const TObject*>(object);
        assert(obj); // This should never fail.
        std::invoke(_setter, std::any_cast<TProp>(value));
    }
};

}