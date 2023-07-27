#include "NoodleToken.hpp"

NoodleToken::NoodleToken(NoodleTokenKind kind, const std::string_view lexeme, int linePos, int characterPos) noexcept
    : _kind(kind)
    , _linePos(linePos)
    , _characterPos(characterPos)
    , _lexeme(lexeme)
{
}