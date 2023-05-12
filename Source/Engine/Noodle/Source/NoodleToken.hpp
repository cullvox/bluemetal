#pragma once

#include "Noodle/Precompiled.hpp"
#include "Noodle/Export.h"

enum class blNoodleTokenKind
{
    eUnexpected,
    eIdentifier,
    eInteger,
    eFloat,
    eBoolean,
    eString,
    eLeftCurly,
    eRightCurly,
    eLeftBracket,
    eRightBracket,
    eEqual,
    eComma,
    eEnd,
    eComment,
    eNewLine,
};

class BLUEMETAL_NOODLE_API blNoodleToken
{
public:
    blNoodleToken() = default;
    blNoodleToken(blNoodleTokenKind kind, const std::string_view lexeme, int linePos, int characterPos) noexcept;

    blNoodleTokenKind kind() const noexcept { return _kind; }
    std::string_view lexeme() const noexcept { return _lexeme; }
    int linePos() const noexcept { return _linePos; }
    int characterPos() const noexcept { return _characterPos; }
    bool is(blNoodleTokenKind kind) const noexcept { return _kind == kind; }

    template <typename... Ts>
    inline bool isOneOf(Ts... ks) const noexcept 
    {
        return (is(ks) || ...);
    }
private:
    std::string_view        _lexeme;
    blNoodleTokenKind       _kind;
    int                     _linePos;
    int                     _characterPos;
};