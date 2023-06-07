#include "NoodleToken.hpp"

blNoodleToken::blNoodleToken(blNoodleTokenKind kind, const std::string_view lexeme, int linePos, int characterPos) noexcept
    : _kind(kind)
    , _linePos(linePos)
    , _characterPos(characterPos)
    , _lexeme(lexeme)
{
}