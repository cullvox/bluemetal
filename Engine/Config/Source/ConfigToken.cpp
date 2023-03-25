#include "Config/ConfigToken.hpp"

namespace bl
{

CToken::CToken(ETokenKind kind, const std::string_view lexeme, int lineNumber) noexcept
    : m_kind(kind), m_lexeme(lexeme), m_lineNumber(lineNumber)
{
}

ETokenKind CToken::Kind() const noexcept
{
    return m_kind;
}

std::string_view CToken::Lexeme() const noexcept
{
    return m_lexeme;
}

} // namespace bl