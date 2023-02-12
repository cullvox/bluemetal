#pragma once

#include <string_view>

enum class ETokenKind
{
    eNumber,
    eIdentifier,
    eString,
    eLeftCurly,
    eRightCurly,
    eEqual,
    eComma,
    eEnd,
    eUnexpected,
};

class CToken
{
public:
    CToken() noexcept = default;
    CToken(ETokenKind kind, const std::string_view lexeme) noexcept;
    
    CToken&          operator=(const CToken& other) = default;

    ETokenKind       Kind() const noexcept;
    std::string_view Lexeme() const noexcept;

private:
    ETokenKind              m_kind;
    std::string_view        m_lexeme;
};
