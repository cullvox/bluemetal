#pragma once

#include "NoodleToken.hpp"
#include "Config/Export.h"

#include <string>
#include <string_view>

class BLUEMETAL_CONFIG_API blNoodleLexerUtils
{
public:
    static bool isSpace(char c) noexcept;
    static bool isDigit(char c) noexcept;
    static bool isIdentifierChar(char c) noexcept;
};

class BLUEMETAL_CONFIG_API blNoodleLexer {
public:
    blNoodleLexer(const std::string& content) noexcept;

    blNoodleToken next() noexcept;
    int linePos() const noexcept;
    int characterPos() const noexcept;
private:
    blNoodleToken createIdentifier() noexcept;
    blNoodleToken createString() noexcept;
    blNoodleToken createNumber() noexcept;
    blNoodleToken createAtom(blNoodleTokenKind kind) noexcept;
    char peek() const noexcept;
    char get() noexcept;

    const std::string& _content;
    std::string::const_iterator _it;
    int _linePos;
    int _characterPos;
};