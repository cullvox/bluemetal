#pragma once

#include <string_view>

#include "Config/Export.h"

enum class blTokenKind
{
    eNumber,
    eIdentifier,
    eString,
    eLeftCurly,
    eRightCurly,
    eEqual,
    eComma,
    eEnd,
    eComment,
    eNewLine,
    eUnexpected,
};

class BLOODLUST_CONFIG_API blToken
{
public:
    blToken() noexcept = default;
    blToken(blTokenKind kind, const std::string_view lexeme, int lineNumber, int lineCharacter) noexcept;

    blTokenKind getKind() const noexcept
    {
        return _kind;
    }

    std::string_view getLexeme() const noexcept
    {
        return _lexeme;
    }

    int getLineNumber() const noexcept
    {
        return _lineNumber;
    }

    int getCharacterNumber() const noexcept
    {
        return _characterNumber;
    }

    inline bool is(blTokenKind kind) const noexcept
    {
        return _kind == kind;
    }

    template <typename... Ts>
    inline bool isOneOf(blTokenKind k1, blTokenKind k2, Ts... ks) const noexcept 
    {
        return is(k1) || isOneOf(k2, ks...);
    }

private:
    std::string_view        _lexeme;
    blTokenKind             _kind;
    int                     _lineNumber;
    int                     _characterNumber;
};