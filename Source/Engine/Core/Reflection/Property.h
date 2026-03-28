#pragma once

#include <string_view>
#include "Core/Variant.h"
#include "Core/Object.h"
#include "Core/Print.h"

namespace bl
{

class Object;

class Property
{
    std::string_view _name;
    VariantType _type;

protected:
    constexpr Property(std::string_view name, VariantType type)
        : _name(name)
        , _type(type)
    {
    }

public:
    Property()
    {
    }

    virtual ~Property()
    {
    }

    constexpr std::string_view GetName() { return _name; }
    constexpr VariantType GetType() { return _type; }

    virtual void Set(Object* object, const Variant& value) = 0;
    virtual Variant Get(Object* object) = 0;
};

template<class T>
concept NonConst = !std::is_const_v<T>;

template<NonConst TClass, typename TValue>
class TProperty : public Property
{
    void (TClass::* _setter)(TValue);
    TValue (TClass::* _getter)(void);
public:
    constexpr TProperty(const std::string_view name, void (TClass::* setter)(TValue), TValue (TClass::* getter)(void))
        : Property(name, GetVariantType<TValue>())
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TProperty()
    {
    }

    virtual void Set(Object* object, const Variant& value)
    {
        if (value.index() != VariantTypeIndex<Variant, TValue>())
        {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return;
        }

        (static_cast<TClass*>(object)->*_setter)(std::get<TValue>(value));
    }

    virtual Variant Get(Object* object)
    {
        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        return (static_cast<TClass*>(object)->*_getter)();
    }
};

template<typename TClass>
class TStringProperty : public Property
{
    using SetterType = void (TClass::*)(const std::string&);
    using GetterType = const std::string& (TClass::*)(void);

    SetterType _setter;
    GetterType _getter;
public:
    constexpr TStringProperty(const std::string_view name, SetterType setter, GetterType getter)
        : Property(name, GetVariantType<std::string>())
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TStringProperty()
    {
    }

    virtual void Set(Object* object, const Variant& value)
    {
        if (value.index() != VariantTypeIndex<Variant, std::string>())
        {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return;
        }

        (static_cast<TClass*>(object)->*_setter)(std::get<std::string>(value));
    }

    virtual Variant Get(Object* object)
    {
        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        return (static_cast<TClass*>(object)->*_getter)();
    }
};

template<NonConst TClass, typename TValue>
TProperty(std::string_view, void (TClass::*)(TValue), TValue (TClass::*)(void)) -> TProperty<TClass, TValue>;

} // namespace bl