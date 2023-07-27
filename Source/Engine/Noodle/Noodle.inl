
#include "NoodleExceptions.h"

namespace ntu
{
    template <typename T, typename... U>
    struct is_any : std::disjunction<std::is_same<T, U>...> {};
}

template<typename T> 
Noodle::Noodle(const std::string& name, const std::vector<T>& value, Noodle* parent)
    : _name(name)
    , _type(NoodleType::eArray)
    , _parent(parent)
{


    static_assert(ntu::is_any<T, int, float, bool>() && "Must be one of the types!");

    for (const T& v : value)
    {
        _array.push_back(Noodle("", v, parent));
    }    
}

template<typename T> 
T& Noodle::get()
{
    if (_type != NoodleType::eInteger &&
        _type != NoodleType::eFloat &&
        _type != NoodleType::eBoolean &&
        _type != NoodleType::eString)
    {
        throw NoodleInvalidAccessException("get expected integer, float, boolean, or string type");
    }

    static_assert(ntu::is_any<T, int, float, bool, std::string>() && "Must be one of the types!");
    return std::get<T>(_value);
}

template<typename T> 
Noodle& Noodle::operator=(const std::vector<T>& value) noexcept
{
    
    return *this;
}
