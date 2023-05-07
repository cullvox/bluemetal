#pragma once

#include <vector>
#include <unordered_map>


#include "Config/Export.h"
#include "ConfigLexer.hpp"
#include "ConfigParserTree.hpp"

enum class blParsedType
{
    eInt,
    eString,
};

class BLOODLUST_CONFIG_API blParser
{
public:
    blParser(const std::string& content) noexcept;
    ~blParser() = default;

    std::unordered_map<std::string, blParsedValue> getValues();
    const blParserTree& getTree();
private:
    void printError(const std::string& expected);
    void next();
    void parse();

    blLexer         _lexer;
    blToken         _token;
    blTokenKind     _tokenKind;
    blParserTree    _tree;
};