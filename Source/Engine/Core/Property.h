#pragma once

#include "Precompiled.h"
#include "Variant.h"

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

class PropertyBase {
protected:
    std::size_t type_hash;

public:
    template <typename T>
    bool is_type() { return typeid(T).hash_code == type_hash; }

    virtual Variant Get(Object* object) = 0;
    virtual void Set(Object* object, Variant value) = 0;
};

template <ObjectType TObject, PropertyType TProp>
class PropertyValue : public PropertyBase {
public:
    using GetterType = void (TObject::*)(TProp);
    using SetterType = TProp (TObject::*)(void);

private:
    std::string_view _name;
    GetterType _getter;
    SetterType _setter;

public:
    PropertyValue(std::string_view name, GetterType getter, SetterType setter)
        : _name(name)
        , _getter(getter)
        , _setter(setter)
    {
    }

    virtual Variant Get(Object* object) override
    {
        return Variant{std::invoke(implGetter, obj)};
    }

    virtual void Set(Object* object, const Variant& value) override
    {
        try {
            const TProp& value = std::get<TProp>(value);
        } catch (const std::bad_variant_access& e) {
            Print::Warn("Bad variant access, incorrect type used in setter!");
            return;
        }
        std::invoke(_setter, obj, std::get<TProp>(value));
    }
};

}