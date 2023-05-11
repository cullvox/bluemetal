#pragma once

#include <string_view>

#include "Config/Export.h"

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
    eEqual,
    eComma,
    eEnd,
    eComment,
    eNewLine,
};

class BLUEMETAL_CONFIG_API blNoodleToken
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
    bool isAny(blNoodleTokenKind k1, blNoodleTokenKind k2, Ts... ks) const noexcept { return is(k1) || isOneOf(k2, ks...); }

private:
    std::string_view        _lexeme;
    blNoodleTokenKind       _kind;
    int                     _linePos;
    int                     _characterPos;
};