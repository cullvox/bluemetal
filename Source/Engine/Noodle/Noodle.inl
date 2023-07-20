
#include "NoodleExceptions.hpp"

namespace ntu
{
    template <typename T, typename... U>
    struct is_any : std::disjunction<std::is_same<T, U>...> {};
}

template<typename T> 
blNoodle::blNoodle(const std::string& name, const std::vector<T>& value, blNoodle* parent)
    : _name(name)
    , _type(blNoodleType::eArray)
    , _parent(parent)
{


    static_assert(ntu::is_any<T, int, float, bool>() && "Must be one of the types!");

    for (const T& v : value)
    {
        _array.push_back(blNoodle("", v, parent));
    }    
}

template<typename T> 
T& blNoodle::get()
{
    if (_type != blNoodleType::eInteger &&
        _type != blNoodleType::eFloat &&
        _type != blNoodleType::eBoolean &&
        _type != blNoodleType::eString)
    {
        throw blNoodleInvalidAccessException("get expected integer, float, boolean, or string type");
    }

    static_assert(ntu::is_any<T, int, float, bool, std::string>() && "Must be one of the types!");
    return std::get<T>(_value);
}

template<typename T> 
blNoodle& blNoodle::operator=(const std::vector<T>& value) noexcept
{
    
    return *this;
}
