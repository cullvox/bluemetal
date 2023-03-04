#pragma once

#include <string_view>
class ConfigSaver
{
public:
    ConfigSaver();

    std::vector<std::string_view> getGroupsFromName(const std::string& name);
};