#pragma once

#include "Precompiled.h"

#include "MacroUtils.h"
#include "Object.h"


/**
 * Any class could have a getter and setter function for any number of parameters.
 *  Each parameter has only one getter and one setter.
 *  Each getter has only one return.
 *  Each setter has only one parameter.
 */


struct _MethodData {

    std::any MethodImplementation()

}

class Method {
    std::string _name;
    std::any _function;

public:

    template<typename T, typename FRet, typename...FArgs>
    Method(FRet (T:: *func)(FArgs&&...args))
    {
        _function = 
        _function = func;
    }

    template<typename...TArgs>
    std::any Call(TArgs...args)
    {

    }

};

#define REGISTER_PARAMETER(param, getter, setter) \
    Parameter(BL_STRINGIFY(param), )


struct ParameterType
{
    
    template<typename T>
    void GetterImpl(T& value)
    {
        
    }

    template<typename T>
    void SetterImpl(T value)
    {

    }

    void* (* getter)();
    void (* setter)(void*);
};




class Parameter {

    std::string _name;
    
    struct _ParameterImpl {

        using Getter = std::any (*)(Object*);
        using Setter = void (*)(Object*, std::any); 

        Getter getter;
        Setter setter;

        template<typename TClass, typename TParameter>
        static std::any GetterImpl(Object* obj)
        {
            
        }

    };

    constexpr _ParameterImpl

public:

    template<class TClass, typename TParameter>
    Parameter(const std::string& name, TClass* obj, TParameter (TClass:: *getter)(), void (TClass:: *setter)(TParameter))
        : _name(name)
        , _getter(static_cast<void*(*)()>(getter))
        , _setter(static_cast<void(*)(void*)>(setter))
    {
    }

    std::any Get()
    {
        return _getter()
    }

    void Set(std::any value)
    {
        
    }

};


class Tester {
    
    float _val;

public:
    float getVal() { return _val; }
    void setVal(float val) { _ val = val; }

    static void RegisterMembers()
    {
        
        Parameter("val", )
    }

}

class ObjectClass {
    
};