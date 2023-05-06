#include "Config/ConfigToken.hpp"

blToken::blToken(blTokenKind kind, const std::string_view lexeme, int lineNumber, int characterNumber) noexcept
    : _kind(kind)
    , _lexeme(lexeme)
    , _lineNumber(lineNumber)
    , _characterNumber(characterNumber)
{
}