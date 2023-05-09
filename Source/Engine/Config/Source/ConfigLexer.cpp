#include "Config/ConfigLexer.hpp"

bool blLexerUtils::IsSpace(char c) noexcept
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

bool blLexerUtils::IsDigit(char c) noexcept
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

bool blLexerUtils::IsIdentifierChar(char c) noexcept
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
// blLexer
//==============================================================================

blLexer::blLexer(const std::string& content) noexcept
    : _content(content)
    , _it(content.begin())
    , _lineNumber(0)
    , _characterNumber(0)
{
}

blToken blLexer::next() noexcept
{

    if (_it == _content.end())
    {
        return blToken(blTokenKind::eEnd, std::string_view(), _lineNumber, _characterNumber);
    }

    // Removes whitespaces and comments, but keeps track of line/character numbers.
    while (true)
    {
        // Remove any comments until endl or eof 
        if (peek() == '#')
        {
            while(peek() != '\n' || peek() == '\0')
            {
                get();
            }
            _lineNumber++;
            get();
        }

        // Remove any spaces
        else if (blLexerUtils::IsSpace(peek()))
        {
            _characterNumber++;

            if (peek() == '\n')
            {
                _characterNumber = 0; 
                _lineNumber++;
            }

            get();
        }

        // Stop on reached anything else
        else
        {
            /* Encountered a token that is not a comment or space. */
            break;
        }
    }

    switch (peek())
    {
        case '\0':
            return blToken(
                blTokenKind::eEnd, 
                std::string_view(_content), 
                getLineNumber(), getCharacterNumber());
        default:
            return createAtom(blTokenKind::eUnexpected);
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
            return createIdentifier();
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
            return createNumber();
        case '{':
            return createAtom(blTokenKind::eLeftCurly);
        case '}':
            return createAtom(blTokenKind::eRightCurly);
        case '=':
            return createAtom(blTokenKind::eEqual);
        case '"':
            return createString();
        case ',':
            return createAtom(blTokenKind::eComma);
    }

    _characterNumber++;
}

int blLexer::getLineNumber() const noexcept
{
    return _lineNumber;
}

int blLexer::getCharacterNumber() const noexcept
{
    return _characterNumber;
}

blToken blLexer::createIdentifier() noexcept
{
    std::string::const_iterator start = _it;
    
    // Read until the end of the identifier
    get();
    while (blLexerUtils::IsIdentifierChar(peek())) get();

    return blToken(
        blTokenKind::eIdentifier, 
        std::string_view{start, _it}, 
        getLineNumber(), getCharacterNumber());
}

blToken blLexer::createString() noexcept
{
    std::string::const_iterator start = _it;

    // Read until the second quote
    get();
    while (peek() != '\"' && peek() != '\0') get();
    get();

    // Checks if the end was actually a double quote, if it wasn't change the string to unknown. 
    bool valid = (*(_it-1) != '\"');

    return blToken{
        valid ? blTokenKind::eUnexpected : blTokenKind::eString,
        std::string_view{start+1, _it-1}, // Contents of string without the "quotes"
        getLineNumber(), getCharacterNumber()};
}

blToken blLexer::createNumber() noexcept
{
    std::string::const_iterator start = _it;

    // Read until the last digit
    get();
    while (blLexerUtils::IsDigit(peek())) get();
    
    return blToken{
        blTokenKind::eNumber,
        std::string_view{start, _it},
        getLineNumber(), getCharacterNumber()};
}

blToken blLexer::createAtom(blTokenKind kind) noexcept
{
    return blToken{
        kind, 
        std::string_view{_it++, _it},
        getLineNumber(), getCharacterNumber()};
}


char blLexer::peek() const noexcept
{
    if (_it == _content.end())
        return '\0';
    else
        return *_it; 
}