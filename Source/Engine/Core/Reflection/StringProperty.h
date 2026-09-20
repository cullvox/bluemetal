#include "Property.h"

namespace bl {

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

    virtual TStringProperty<TClass>* Clone() const override
    {
        return new TStringProperty<TClass>(GetName(), GetFlags(), _setter, _getter);
    }

    virtual void Set(Object* object, Variant value) override
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

    virtual Variant Get(Object* object) override
    {
        if (dynamic_cast<TClass*>(object) == nullptr)
        {
            Print::Error("Invalid object ({}) on property getter class ({}).", object->GetClassName(), TClass::GetStaticClassName());
            return Variant{};
        }

        return (static_cast<TClass*>(object)->*_getter)();
    }
};

} // namespace bl