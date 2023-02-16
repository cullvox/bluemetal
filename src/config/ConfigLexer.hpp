#pragma once

#include <string>
#include <string_view>

#include "ConfigToken.hpp"

class CLexerUtils
{
public:
    static bool IsSpace(char c) noexcept;
    static bool IsDigit(char c) noexcept;
    static bool IsIdentifierChar(char c) noexcept;
};

class CLexer {
public:
    CLexer(const std::string& content) noexcept;

    CToken Next() noexcept;
    int Line() const noexcept;
private:
    CToken Identifier() noexcept;
    CToken String() noexcept;
    CToken Number() noexcept;
    CToken Atom(ETokenKind kind) noexcept;
    inline char Peek() const noexcept { return *m_it; }
    inline char Get() noexcept { return *m_it++; }

    const std::string& m_content;
    std::string::const_iterator m_it;
    int m_lineNumber;
};