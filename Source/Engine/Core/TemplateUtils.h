#pragma once

namespace bl {

template<typename T, typename... U>
struct is_any : std::disjunction<std::is_same<T, U>...> {};

}; // namespace bl
