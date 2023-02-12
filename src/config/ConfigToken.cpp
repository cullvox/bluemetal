#include "ConfigToken.hpp"

CToken::CToken(ETokenKind kind, const std::string_view lexeme) noexcept
    : m_kind(kind), m_lexeme(lexeme)
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