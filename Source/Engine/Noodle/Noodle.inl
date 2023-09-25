#include "Core/TemplateUtils.h"

namespace bl {

template<typename T>
Noodle::Noodle(const std::string& name, const std::vector<T>& value, Noodle* parent)
    : _name(name)
    , _type(NoodleType::eArray)
    , _parent(parent)
{
    static_assert(bl::is_any<T, int, float, bool, std::string>() && "T must be int, float, bool or std::string!");

    for (const T& v : value)
    {
        _array.push_back(Noodle("", v, parent));
    }
}

template<typename T>
T& Noodle::get()
{
    static_assert(bl::is_any<T, int, float, bool, std::string>() && "T must be int, float, bool or std::string!");

    if (_type != NoodleType::eInteger && _type != NoodleType::eFloat && _type != NoodleType::eBoolean && _type != NoodleType::eString)
    {
        throw NoodleInvalidAccessException("get expected integer, float, boolean, or string type");
    }

    return std::get<T>(_value);
}

template<typename T>
T& Noodle::operator[](size_t index)
{
    static_assert(bl::is_any<T, int, float, bool, std::string>() && "T must be int, float, bool or std::string!");
    
    if (_type != NoodleType::eArray) {
        throw std::runtime_error("Cannot access an array from a non array noodle!");
    }
    
    std::vector<T>& arr = _array.get<std::vector<T>>();
    return arr[index];
}

template<typename T>
const T& Noodle::operator[](size_t index) const
{
    static_assert(bl::is_any<T, int, float, bool, std::string>() && "T must be int, float, bool or std::string!");
    
    if (_type != NoodleType::eArray) {
        throw std::runtime_error("Cannot access an array from a non array noodle!");
    }
    
    const std::vector<T>& arr = _array.get<std::vector<T>>();
    return arr[index];
}

template<typename T>
Noodle& Noodle::operator=(const std::vector<T>& value)
{

    return *this;
}

template<>
Noodle& Noodle::operator=(int value)
{
    _type = NoodleType::eInteger;
    _value = value;
    return *this;
}

template<>
Noodle& Noodle::operator=(float value)
{
    _type = NoodleType::eFloat;
    _value = value;
    return *this;
}

template<>
Noodle& Noodle::operator=(bool value)
{
    _type = NoodleType::eBoolean;
    _value = value;
    return *this;
}

template<>
Noodle& Noodle::operator=(const std::string& value)
{
    _type = NoodleType::eString;
    _value = value;
    return *this;
}

template<typename T>
Noodle& Noodle::operator=(T value)
{
    static_assert(false && "Type must be any of int, float, bool, std::string or array variants!");
    return Noodle();
}

} // namespace bl
