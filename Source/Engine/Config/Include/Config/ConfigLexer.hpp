#pragma once

#include "ConfigToken.hpp"
#include "Config/Export.h"

#include <string>
#include <string_view>

class BLOODLUST_CONFIG_API blLexerUtils
{
public:
    static bool IsSpace(char c) noexcept;
    static bool IsDigit(char c) noexcept;
    static bool IsIdentifierChar(char c) noexcept;
};

class BLOODLUST_CONFIG_API blLexer {
public:
    blLexer(const std::string& content) noexcept;

    blToken next() noexcept;
    int getLineNumber() const noexcept;
    int getCharacterNumber() const noexcept;
private:
    blToken createIdentifier() noexcept;
    blToken createString() noexcept;
    blToken createNumber() noexcept;
    blToken createAtom(blTokenKind kind) noexcept;
    inline char peek() const noexcept { return *_it; }
    inline char get() noexcept { return *_it++; }

    const std::string& _content;
    std::string::const_iterator _it;
    int _lineNumber;
    int _characterNumber;
};