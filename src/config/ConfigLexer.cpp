#include "ConfigLexer.hpp"





//==============================================================================
// CLexerUtils
//==============================================================================

bool CLexerUtils::IsSpace(char c) noexcept
{
    switch (c)
    {
        case ' ':
        case '\t':
        case '\r':
        case '\n':
            return true;
        default:
            return false;
    }
}

bool CLexerUtils::IsDigit(char c) noexcept
{
    switch (c)
    {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return true;
        default:
            return false;
    }
}

bool CLexerUtils::IsIdentifierChar(char c) noexcept
{
    switch (c)
    {
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '_':
            return true;
        default:
            return false;
    }
}





//==============================================================================
// CLexer
//==============================================================================

CLexer::CLexer(const std::string& content) noexcept
    : m_content(content)
{
    m_it = content.begin();
}

CToken CLexer::Next() noexcept
{

    /* Removes whitespaces and comments, but keeps track of line counts. */
    while (true)
    {
        if (Peek() == '#')
        {
            /* Keep going until end line or eof. */
            while(Peek() != '\n' || Peek() == '\0')
            {   
                Get();
            }
            m_lineNumber++;
            Get();
        }
        else if (CLexerUtils::IsSpace(Peek()))
        {
            if (Peek() == '\n') m_lineNumber++;
            Get();
        }
        else
        {
            /* Encountered a token that is not a comment or space. */
            break;
        }
    }

    switch (Peek())
    {
        case '\0':
            return CToken(ETokenKind::eEnd, std::string_view(m_it, m_it+1), 0);
        default:
            return Atom(ETokenKind::eUnexpected);
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
            return Identifier();
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            return Number();
        case '{':
            return Atom(ETokenKind::eLeftCurly);
        case '}':
            return Atom(ETokenKind::eRightCurly);
        case '=':
            return Atom(ETokenKind::eEqual);
        case '"':
            return String();
        case ',':
            return Atom(ETokenKind::eComma);
    }
}

CToken CLexer::Identifier() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (CLexerUtils::IsIdentifierChar(Peek())) Get();
    return CToken(ETokenKind::eIdentifier, std::string_view{start, m_it}, m_lineNumber);
}

CToken CLexer::String() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (Peek() != '\"' && Peek() != '\0') Get();
    Get();

    return CToken{
        (*(m_it-1) != '\"') ? ETokenKind::eUnexpected : ETokenKind::eString, /* Checks if the end was actually a double quote, if it wasn't change the string to unknown. */ 
        std::string_view{start+1, m_it-1}, /* Contents of string. */
        m_lineNumber};
}

CToken CLexer::Number() noexcept
{
    std::string::const_iterator start = m_it;
    Get();
    while (CLexerUtils::IsDigit(Peek())) Get();
    return CToken{
        ETokenKind::eNumber,
        std::string_view{start, m_it},
        m_lineNumber};
}

CToken CLexer::Atom(ETokenKind kind) noexcept
{
    return CToken{
        kind, 
        std::string_view{m_it++, m_it},
        m_lineNumber};
}

int CLexer::Line() const noexcept
{
    return m_lineNumber;
}