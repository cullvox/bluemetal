#pragma once

#include <string_view>
#include "Core/Variant.h"
#include "Core/Object.h"
#include "Core/Print.h"

namespace bl
{

/// - Change the way a property is intended to be used or percieved.
enum class PropertyFlags : uint8_t
{
    
    /// - No property flags will be set.
    None = 0,
    
    /// - When a property is marked as 'Editor' it will be visible in the inspector
    /// as a potentially editable value. You can combine this flag with 'ReadOnly'
    /// to create an editor visible read only property.
    Editor = 1 << 0,

    /// - When a property is marked as 'ReadOnly', the property system will not 
    /// allow the value to be changed, the 'Set' operation is completely skipped. 
    /// - The only time a value marked as 'ReadOnly' can ever be changed is during
    /// a load time, such as a scene load.
    ReadOnly = 1 << 1,

    /// - When a property is serialized it means that it saves it to file on 
    /// export. When you save a scene in editor, all serializable properties are 
    /// exported in the save process.
    /// - When a property is not marked as 'Serialize' it's contents are not saved
    /// in any way in an export or save of any kind.
    Serialize = 1 << 2
};

inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b) { return static_cast<PropertyFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
inline PropertyFlags operator&(PropertyFlags a, PropertyFlags b) { return static_cast<PropertyFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
inline PropertyFlags operator~(PropertyFlags a)                  { return static_cast<PropertyFlags>(~static_cast<uint8_t>(a)); }

/// - A member of a class to be set, changed, or viewed by various systems.
class Property
{
    ClassDB& _db;
    std::string_view _name;
    PropertyFlags _flags;
    VariantType _type;

protected:
    Property(ClassDB& db, std::string_view name, PropertyFlags flags, VariantType type)
        : _db(db)
        , _name(name)
        , _flags(flags)
        , _type(type)
    {
    }

    ClassDB& GetClassDB() { return _db; }

public:
    virtual ~Property() = default;

    std::string_view GetName()          { return _name; }
    VariantType GetType()               { return _type; }
    bool HasFlag(PropertyFlags flag)    { return (_flags & flag) == flag; }
    PropertyFlags GetFlags()            { return _flags; }

    virtual void Set(Object* object, const Variant& value) = 0;
    virtual Variant Get(Object* object) = 0;
};

template<class T>
concept NonConst = !std::is_const_v<T>;

/// - A general property instance for most basic property types.
template<NonConst TClass, typename TValue>
class TProperty : public Property
{
    void (TClass::* _setter)(TValue);
    TValue (TClass::* _getter)(void);
public:
    constexpr TProperty(ClassDB& db, const std::string_view name, PropertyFlags flags, void (TClass::* setter)(TValue), TValue (TClass::* getter)(void))
        : Property(db, name, flags, GetVariantType<TValue>())
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
    constexpr TStringProperty(ClassDB& db, const std::string_view name, PropertyFlags flags, SetterType setter, GetterType getter)
        : Property(db, name, flags, GetVariantType<std::string>())
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