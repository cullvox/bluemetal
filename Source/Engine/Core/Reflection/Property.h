#pragma once

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
    Serialize = 1 << 2,

    /// - When a property is marked as 'Normalize' it means that the engine will
    /// keep the value normalized during sets. This is only used for properties 
    /// that can be normalized such as quaternions and vectors.
    Normalize = 1 << 3,

    /// - When a property is marked as 'Color' it means that the editor should
    /// display a color picker for this property instead of the normal editor
    /// for the type. This is only used for glm::vec3 and glm::vec4 properties.
    Color = 1 << 4
};

inline PropertyFlags operator|(PropertyFlags a, PropertyFlags b) { return static_cast<PropertyFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); }
inline PropertyFlags operator&(PropertyFlags a, PropertyFlags b) { return static_cast<PropertyFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); }
inline PropertyFlags operator~(PropertyFlags a)                  { return static_cast<PropertyFlags>(~static_cast<uint8_t>(a)); }

inline PropertyFlags& operator|=(PropertyFlags& a, PropertyFlags b) { a = static_cast<PropertyFlags>(static_cast<uint8_t>(a) | static_cast<uint8_t>(b)); return a; }
inline PropertyFlags& operator&=(PropertyFlags& a, PropertyFlags b) { a = static_cast<PropertyFlags>(static_cast<uint8_t>(a) & static_cast<uint8_t>(b)); return a; }

/// - A member of a class to be set, changed, or viewed by various systems.
class Property
{
    std::string_view _name;
    PropertyFlags _flags;
    VariantType _type;

protected:
    Property(std::string_view name, PropertyFlags flags, VariantType type)
        : _name(name)
        , _flags(flags)
        , _type(type)
    {
    }

public:
    virtual ~Property() = default;

    std::string_view GetName()          { return _name; }
    VariantType GetType()               { return _type; }
    bool HasFlag(PropertyFlags flag)    { return (_flags & flag) == flag; }
    PropertyFlags GetFlags()            { return _flags; }

    virtual void Set(Object* object, Variant value) = 0;
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

    // Handle object pointers as a special case since we want to be able to use them for any object type, but they are all stored as Object* in the variant.
    using Type = std::conditional_t<std::is_pointer_v<TValue> && std::is_base_of_v<Object, std::remove_pointer_t<TValue>>, Object*, TValue>;

public:
    constexpr TProperty(const std::string_view name, PropertyFlags flags, void (TClass::* setter)(TValue), TValue (TClass::* getter)(void))
        : Property(name, flags, GetVariantType<TValue>())
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TProperty()
    {
    }

    virtual void Set(Object* object, Variant value)
    {
        if (value.index() != VariantTypeIndex<Variant, Type>())
        {
            Print::Error("Could not set property, ({}) invalid type on class ({}).", GetName(), object->GetClassName());
            return;
        }

        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property setter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
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

        if constexpr (std::is_pointer_v<TValue> && std::is_base_of_v<Object, std::remove_pointer_t<TValue>>) {
            // If this is an object pointer, we need to cast it to the correct type before setting it.
            Object* obj = std::get<Type>(value);
            if (obj && !obj->IsA(TClass::GetStaticClassName())) {
                Print::Error("Invalid object type ({}) on property setter class ({}).", obj->GetClassName(), TClass::GetStaticClassName());
                return;
            }
            (static_cast<TClass*>(object)->*_setter)(static_cast<TValue>(obj));
        } else {
            (static_cast<TClass*>(object)->*_setter)(std::get<Type>(value));
        }
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
    constexpr TStringProperty(const std::string_view name, PropertyFlags flags, SetterType setter, GetterType getter)
        : Property(name, flags, GetVariantType<std::string>())
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TStringProperty()
    {
    }

    virtual void Set(Object* object, Variant value)
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