#pragma once

#include <string>
#include <variant>
#include <unordered_map>

namespace bl
{

class EngineVars
{
    using Var = std::variant<int32_t, int64_t, uint32_t, uint64_t, bool, std::string, float>;

    std::unordered_map<std::string, Var> _vars;
public:

    template<typename T>
    T Get(const std::string& name)
    {
        if (_vars.contains(name)) {
            return std::get<T>(_vars[name]);
        } else {
            // Print::Error("Invalid engine variable.");
            return {};
        }

    }

    template<typename T>
    void Set(std::string_view name, T value)
    {
        //if (_vars[name].)
    }

    template<typename T>
    void Set(std::string_view name, const T& value);

};

}