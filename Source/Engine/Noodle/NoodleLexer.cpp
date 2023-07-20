#include "NoodleLexer.hpp"

bool blNoodleLexerUtils::isSpace(char c) noexcept
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

bool blNoodleLexerUtils::isDigit(char c) noexcept
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
        case '.':
            return true;
        default:
            return false;
    }
}

bool blNoodleLexerUtils::isIdentifierChar(char c) noexcept
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
// blNoodleLexer
//==============================================================================

blNoodleLexer::blNoodleLexer(const std::string& content) noexcept
    : _content(content)
    , _it(content.begin())
    , _linePos(0)
    , _characterPos(0)
{
}

blNoodleToken blNoodleLexer::next() noexcept
{

    if (_it == _content.end())
    {
        return blNoodleToken(blNoodleTokenKind::eEnd, std::string_view(), linePos(), characterPos());
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
            get();
        }

        // Remove any spaces
        else if (blNoodleLexerUtils::isSpace(peek()))
        {
            get();
        }

        // Stop on reached anything else
        else
        {
            // Encountered a token that is not a comment or space.
            break;
        }
    }

    switch (peek())
    {
        case '\0':
            return blNoodleToken(blNoodleTokenKind::eEnd, std::string_view(_content), linePos(), characterPos());
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
        case '[':
            return createAtom(blNoodleTokenKind::eLeftBracket);
        case ']':
            return createAtom(blNoodleTokenKind::eRightBracket);
        case '{':
            return createAtom(blNoodleTokenKind::eLeftCurly);
        case '}':
            return createAtom(blNoodleTokenKind::eRightCurly);
        case '=':
            return createAtom(blNoodleTokenKind::eEqual);
        case '"':
            return createString();
        case ',':
            return createAtom(blNoodleTokenKind::eComma);
        default:
            return createAtom(blNoodleTokenKind::eUnexpected);
    }
}

int blNoodleLexer::linePos() const noexcept
{
    return _linePos;
}

int blNoodleLexer::characterPos() const noexcept
{
    return _characterPos;
}

blNoodleToken blNoodleLexer::createIdentifier() noexcept
{
    std::string::const_iterator start = _it;
    
    // Read until the end of the identifier
    get();
    while (blNoodleLexerUtils::isIdentifierChar(peek())) get();

    
    std::string_view view(start, _it);
    blNoodleTokenKind kind = blNoodleTokenKind::eIdentifier;

    if (view == "true" || view == "false")
        kind = blNoodleTokenKind::eBoolean;

    return blNoodleToken(kind, view, linePos(), characterPos());
}

blNoodleToken blNoodleLexer::createString() noexcept
{
    std::string::const_iterator start = _it;

    // Read until the second quote
    get();
    while (peek() != '\"' && peek() != '\0') get();
    get();

    // Checks if the end was actually a double quote, if it wasn't change the string to unknown. 
    bool valid = (*(_it-1) == '\"');

    return blNoodleToken(
        valid ? blNoodleTokenKind::eString : blNoodleTokenKind::eUnexpected,
        std::string_view{start+1, _it-1}, // Contents of string without the "quotes"
        linePos(), characterPos());
}

blNoodleToken blNoodleLexer::createNumber() noexcept
{
    std::string::const_iterator start = _it;

    // Read until the last digit
    get();
    while (blNoodleLexerUtils::isDigit(peek())) get();

    // Detect what type the number is
    // This is almost certainly a hack to get this to work
    // I do not like thi &(*start), &(*_it) trick
    char* longEnd;
    char* floatEnd;

    std::strtol(&(*start), &longEnd, 10);
    std::strtof(&(*start), &floatEnd);

    // If the end pointers don't match the current iterator, somethings wrong with the noodle
    blNoodleTokenKind kind = blNoodleTokenKind::eUnexpected;

    if (&(*_it) == longEnd)
        kind = blNoodleTokenKind::eInteger;
    else if (&(*_it) == floatEnd)
        kind = blNoodleTokenKind::eFloat;
    
    return blNoodleToken(kind, std::string_view{start, _it}, linePos(), characterPos());
}

blNoodleToken blNoodleLexer::createAtom(blNoodleTokenKind kind) noexcept
{
    return blNoodleToken(kind, std::string_view{_it++, _it}, linePos(), characterPos());
}

char blNoodleLexer::peek() const noexcept
{
    if (_it == _content.end())
        return '\0';
    else
        return *_it;
}

char blNoodleLexer::get() noexcept
{
    char current = *_it++;

    _characterPos++;
    if (current == '\n')
    {
        _linePos++;
        _characterPos = 0;
    }

    return current;
}