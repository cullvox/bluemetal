#pragma once

#include "Precompiled.h"

namespace bl {

/** @brief All possible tokens the lexer can find. */
enum class NoodleTokenKind {
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

constexpr std::string to_string(NoodleTokenKind value)
{
    switch (value) {
    case bl::NoodleTokenKind::eIdentifier: return "Identifier"; 
    case bl::NoodleTokenKind::eInteger: return "Integer"; 
    case bl::NoodleTokenKind::eFloat: return "Float"; 
    case bl::NoodleTokenKind::eBoolean: return "Boolean"; 
    case bl::NoodleTokenKind::eString: return "String"; 
    case bl::NoodleTokenKind::eLeftCurly: return "LeftCurly"; 
    case bl::NoodleTokenKind::eRightCurly: return "RightCurly"; 
    case bl::NoodleTokenKind::eLeftBracket: return "LeftBracket"; 
    case bl::NoodleTokenKind::eRightBracket: return "RightBracket"; 
    case bl::NoodleTokenKind::eEqual: return "Equal"; 
    case bl::NoodleTokenKind::eComma: return "Comma"; 
    case bl::NoodleTokenKind::eEnd: return "End"; 
    case bl::NoodleTokenKind::eComment: return "Comment"; 
    case bl::NoodleTokenKind::eNewLine: return "NewLine";
    case bl::NoodleTokenKind::eUnexpected:
    default:
         return "Unexpected"; 
    }
}

class NoodleToken {
public:

    /** @brief Default Constructor */
    NoodleToken() = default;

    /** Full Constructor */
    NoodleToken(NoodleTokenKind kind, const std::string_view lexeme, int linePos, int characterPos);

public:

    /** @brief Returns this tokens kind. */
    NoodleTokenKind getKind() const noexcept { return _kind; }
    
    /** @brief Returns the text this token represents. */
    std::string_view getLexeme() const noexcept { return _lexeme; }

    /** @brief Returns the line position of this token. */
    int getRow() const noexcept { return _row; }

    /** @brief Returns the character position of this token. */
    int getCol() const noexcept { return _col; }

    /** @brief Returns true if the tokens kind is equal to kind. */
    bool is(NoodleTokenKind kind) const noexcept { return _kind == kind; }

    /** @brief Returns true if the tokens kind is within the kinds provided. */
    template <typename... Ts>
    inline bool isOneOf(Ts... ks) const noexcept
    {
        return (is(ks) || ...);
    }
    
private:
    NoodleTokenKind     _kind;
    int                 _row;
    int                 _col;
    std::string_view    _lexeme;
};

} // namespace bl

template<>
struct fmt::formatter<bl::NoodleTokenKind> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    format_context::iterator format(bl::NoodleTokenKind kind, format_context& ctx) const
    {
        return fmt::format_to(ctx.out(), "{}", bl::to_string(kind));
    }
};
