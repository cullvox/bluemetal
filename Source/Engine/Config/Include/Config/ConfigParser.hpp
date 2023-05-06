#pragma once

#include <vector>
#include <unordered_map>

#include "ConfigLexer.hpp"
#include "Config/Export.h"

enum class blParsedType
{
    eInt,
    eString,
};

using blParsedValue = std::variant<int, std::string_view>;

class BLOODLUST_CONFIG_API blParser
{
public:
    blParser(const std::string& content) noexcept;

    const std::unordered_map<std::string, blParsedValue>& parse();
private:
    void next();
    void recomputeGroups();
    void printError(const std::string& expected);

    blLexer _lexer;
    blToken _token;
    blTokenKind _tokenKind;
    std::vector<std::string_view> _groups;
    std::string _groupsStr;
    std::unordered_map<std::string, blParsedValue> _values;
};