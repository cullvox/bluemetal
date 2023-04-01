#pragma once

#include <string_view>

#include "Generated/Export_Config.h"

namespace bl
{

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
    eComment,
    eNewLine,
    eUnexpected,
};

class BLOODLUST_CONFIG_API CToken
{
public:
    CToken() noexcept = default;
    CToken(ETokenKind kind, const std::string_view lexeme, int lineNumber) noexcept;

    ETokenKind          Kind() const noexcept;
    std::string_view    Lexeme() const noexcept;
    int                 LineNumber() const noexcept;
    int                 LineCharacter() const noexcept;
    inline bool         Is(ETokenKind kind) const noexcept
    {
        return m_kind == kind;
    }
    template <typename... Ts>
    inline bool         IsOneOf(ETokenKind k1, ETokenKind k2, Ts... ks) const noexcept 
    {
        return Is(k1) || IsOneOf(k2, ks...);
    }


private:
    ETokenKind              m_kind;
    int                     m_lineNumber;
    std::string_view        m_lexeme;
};

} // namespace bl