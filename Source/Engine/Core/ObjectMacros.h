#pragma once

#include "MacroUtils.h"

#define CLASS_OBJECT_VIRTUAL(NAME, PARENT) \
public: \
    static std::string_view _ClassName() { return BL_STRINGIFY(NAME); } \
    static std::string_view _ParentClassName() { return BL_STRINGIFY(PARENT); } \
    static const ObjectClass& GetObjectClass() { return *(ObjectClasses::GetObjectClass(BL_STRINGIFY(NAME))); } \
private:

#define CLASS_OBJECT(NAME, PARENT) \
public: \
    static Object* _Creator() { return new NAME(); } \
    static std::string_view ClassName() { return BL_STRINGIFY(NAME); } \
    static std::string_view ParentClassName() { return BL_STRINGIFY(PARENT); } \
    static const ObjectClass& GetObjectClass() { return *(ObjectClasses::GetObjectClass(BL_STRINGIFY(NAME))); } \
private:

#define OBJECT_REGISTRATION(NAME) \
    static ObjectRegistration<NAME> _##NAME_registration;