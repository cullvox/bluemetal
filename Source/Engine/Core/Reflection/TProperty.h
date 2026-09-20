
#include "Core/Reflection/Property.h"
#include "Core/Object.h"
#include "Core/Reference.h"

namespace bl {

template<class T>
concept NonConst = !std::is_const_v<T>;

/// - A general property instance for most basic property types.
template<NonConst TClass, typename TValue>
class TProperty : public Property
{
    void (TClass::* _setter)(TValue);
    TValue (TClass::* _getter)(void);

    template<typename T>
    struct IsObjectReference : std::false_type {};

    template<typename T>
    struct IsObjectReference<Ref<T>> : std::bool_constant<std::is_base_of_v<Object, T>> {};

    static constexpr bool IsObjectPointer = std::is_pointer_v<TValue> && std::is_base_of_v<Object, std::remove_pointer_t<TValue>>;
    static constexpr bool IsObjectRef = IsObjectReference<TValue>::value;

    // Object pointers and references are stored as their base type in the variant.
    using Type = std::conditional_t<IsObjectPointer || IsObjectRef, Ref<Object>, TValue>;

public:
    constexpr TProperty(const std::string_view name, PropertyFlags flags, void (TClass::* setter)(TValue), TValue (TClass::* getter)(void))
        : Property(name, flags, GetVariantType<Type>())
        , _setter(setter)
        , _getter(getter)
    {
    }

    ~TProperty()
    {
    }

    virtual TProperty* Clone() const override
    {
        return new TProperty<TClass, TValue>(GetName(), GetFlags(), _setter, _getter);
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

        if constexpr (IsObjectPointer) {
            // If this is an object pointer, we need to cast it to the correct type before setting it.
            Object* obj = std::get<Type>(value);
            if (obj && !obj->IsA(TClass::GetStaticClassName())) {
                Print::Error("Invalid object type ({}) on property setter class ({}).", obj->GetClassName(), TClass::GetStaticClassName());
                return;
            }
            (static_cast<TClass*>(object)->*_setter)(static_cast<TValue>(obj));
        } else if constexpr (IsObjectRef) {
            (static_cast<TClass*>(object)->*_setter)(std::get<Type>(value).template Cast<typename TValue::ValueType>());
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

        if constexpr (IsObjectRef) {
            return (static_cast<TClass*>(object)->*_getter)().template Cast<Object>();
        } else {
            return (static_cast<TClass*>(object)->*_getter)();
        }
    }
};

template<NonConst TClass, typename TValue>
TProperty(std::string_view, void (TClass::*)(TValue), TValue (TClass::*)(void)) -> TProperty<TClass, TValue>;

} // namespace bl