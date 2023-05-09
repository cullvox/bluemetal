#pragma once

#include "Core/Precompiled.hpp"
#include "Config/ConfigParser.hpp"
#include "Config/ConfigParserTree.hpp"
#include "Config/Export.h"

class BLOODLUST_CONFIG_API blConfig
{
public:
    
    blConfig(const std::string& defaultConfig, const std::filesystem::path& path);
    ~blConfig();

    bool hasValue(const std::string&) const noexcept;
    template<typename T> T& get(const std::string& key);
    template<typename T> T get(const std::string& key) const;
    template<typename T> T& operator[](const std::string& key);
    template<typename T> T operator[](const std::string& key) const;
    
    void reset();
    void save();

private:

    void ensurePathExists();
    void resetIfPathDoesNotExist();
    void parseInto();
    void groupValues();

    std::string                                     _defaultConfig;
    std::filesystem::path                           _path;
    std::unordered_map<std::string, blParsedValue>  _values;
};

////////////////////////////////////////////////////////////////////////////////
#include "Core/TemplateUtils.hpp"

template<typename T>
T& blConfig::get(const std::string& key)
{
    static_assert(bl::is_any<T, int, std::string>::value);
    return std::get<T>(_values[key]);
}

template<typename T>
T& blConfig::operator[](const std::string& key)
{
    return get(key);
}

template<typename T>
T blConfig::get(const std::string& key) const
{
    return std::forward(get(key));
}

template<typename T>
T blConfig::operator[](const std::string& key) const
{
    return std::forward(get(key));
}