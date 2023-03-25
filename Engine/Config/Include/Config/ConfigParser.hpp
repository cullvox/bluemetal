#pragma once

#include <vector>
#include <unordered_map>

#include "ConfigLexer.hpp"

namespace bl
{

enum class EParsedType
{
    eInt,
    eString,
};

struct SParsedValue
{
    EParsedType type;
    long int i;
    std::string_view s;
};

class CParser
{
public:
    struct SParserResult
    {
        bool success;
        std::string error;
    };

    CParser(const std::string& content) noexcept;

    const std::unordered_map<std::string, SParsedValue>& Parse(); // throws on parser error
private:
    void Next();
    void RecomputeGroups();
    void PrintError(const std::string& expected);

    CLexer m_lexer;
    CToken m_token;
    ETokenKind m_tokenKind;
    std::vector<std::string_view> m_groups;
    std::string m_groupsStr;
    std::unordered_map<std::string, SParsedValue> m_values;
};

} // namespace bl