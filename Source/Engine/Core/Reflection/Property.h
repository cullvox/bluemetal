#pragma once

#include "Core/Variant.h"
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
    Property(Property&) = default;
    Property(Property&&) = default;
    virtual ~Property() {}

    std::string_view GetName() const    { return _name; }
    VariantType GetType() const         { return _type; }
    bool HasFlag(PropertyFlags flag)    { return (_flags & flag) == flag; }
    PropertyFlags GetFlags() const      { return _flags; }

    virtual Property* Clone() const { return nullptr; };
    virtual void Set(Object* object, Variant value) = 0;
    virtual Variant Get(Object* object) = 0;
};


} // namespace bl