#pragma once

#include "Precompiled.h"

enum class NoodleTokenKind
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

class NoodleToken
{
public:
    NoodleToken() = default;
    NoodleToken(NoodleTokenKind kind, const std::string_view lexeme, int linePos, int characterPos)
        : _kind(kind)
        , _lexeme(lexeme)
        , _linePos(linePos)
        , _characterPos(characterPos)
    {
    }

    NoodleTokenKind kind() { return _kind; }
    std::string_view lexeme() { return _lexeme; }
    int linePos() { return _linePos; }
    int characterPos() { return _characterPos; }
    bool is(NoodleTokenKind kind) { return _kind == kind; }

    template <typename... Ts>
    inline bool isOneOf(Ts... ks)
    {
        return (is(ks) || ...);
    }
    
private:
    NoodleTokenKind       _kind;
    int                     _linePos;
    int                     _characterPos;
    std::string_view        _lexeme;
};