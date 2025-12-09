#pragma once

#include <any>

namespace bl {

template <typename T, typename... U>
struct is_any : std::disjunction<std::is_same<T, U>...> { };

template <typename T>
T& any_cast_or_default(const std::any& any, T&& def)
{
    try {
        return std::any_cast<T>(std::forward(any));
    } catch (const std::bad_any_cast& e) {
        return def;
    }
}

}; // namespace bl
