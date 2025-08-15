#pragma once

#include "Precompiled.h"

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

    virtual std::any Get() = 0;
    virtual void Set(std::any value) = 0;
};

template <ObjectType TObject, PropertyType TProp>
class PropertyValue : public PropertyBase {
public:
    using GetterType = void (TObject::* _getter)(TProp);
    using SetterType = TProp (TObject::* _setter)(void);

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

    virtual std::any Get() override
    {
        return std::invoke(_getter);
    }

    virtual void Set(std::any value) override
    {
        std::invoke(_setter, std::any_cast<TProp>(value));
    }
};

class Property {
    std::unique_ptr<PropertyBase> _property;
public:
    template <class TClass, typename TProp>
    Property(std::string_view name, std::function<TProp()> getter, std::function<void(TProp)> setter);

    void Set(Object* object, std::any value);
    std::any Get(Object* object);
};

}