#pragma once

#include "Precompiled.h"
#include <unordered_map>

namespace bl {

class Config 
{
public:
    using Option = std::variant<int, bool, float>;

    Config(std::filesystem::path path);
    ~Config();

private:
    std::unordered_map<std::string, Option> _values;

};

}