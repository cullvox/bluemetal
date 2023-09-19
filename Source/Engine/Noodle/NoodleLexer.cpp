#include "NoodleLexer.h"

bool NoodleLexerUtils::isSpace(char c)
{
    switch (c) {
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        return true;
    default:
        return false;
    }
}

bool NoodleLexerUtils::isDigit(char c)
{
    switch (c) {
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

bool NoodleLexerUtils::isIdentifierChar(char c)
{
    switch (c) {
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
// NoodleLexer
//==============================================================================

NoodleLexer::NoodleLexer(const std::string& content)
    : _content(content)
    , _it(content.begin())
    , _linePos(0)
    , _characterPos(0)
{
}

NoodleToken NoodleLexer::next()
{

    if (_it == _content.end()) {
        return NoodleToken(NoodleTokenKind::eEnd, std::string_view(), linePos(), characterPos());
    }

    // Removes whitespaces and comments, but keeps track of line/character numbers.
    while (true) {
        // Remove any comments until endl or eof
        if (peek() == '#') {
            while (peek() != '\n' || peek() == '\0') {
                get();
            }
            get();
        }

        // Remove any spaces
        else if (NoodleLexerUtils::isSpace(peek())) {
            get();
        }

        // Stop on reached anything else
        else {
            // Encountered a token that is not a comment or space.
            break;
        }
    }

    switch (peek()) {
    case '\0':
        return NoodleToken(
            NoodleTokenKind::eEnd, std::string_view(_content), linePos(), characterPos());
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
        return createAtom(NoodleTokenKind::eLeftBracket);
    case ']':
        return createAtom(NoodleTokenKind::eRightBracket);
    case '{':
        return createAtom(NoodleTokenKind::eLeftCurly);
    case '}':
        return createAtom(NoodleTokenKind::eRightCurly);
    case '=':
        return createAtom(NoodleTokenKind::eEqual);
    case '"':
        return createString();
    case ',':
        return createAtom(NoodleTokenKind::eComma);
    default:
        return createAtom(NoodleTokenKind::eUnexpected);
    }
}

int NoodleLexer::linePos() const { return _linePos; }

int NoodleLexer::characterPos() const { return _characterPos; }

NoodleToken NoodleLexer::createIdentifier()
{
    std::string::const_iterator start = _it;

    // Read until the end of the identifier
    get();
    while (NoodleLexerUtils::isIdentifierChar(peek()))
        get();

    std::string_view view(start, _it);
    NoodleTokenKind kind = NoodleTokenKind::eIdentifier;

    if (view == "true" || view == "false")
        kind = NoodleTokenKind::eBoolean;

    return NoodleToken(kind, view, linePos(), characterPos());
}

NoodleToken NoodleLexer::createString()
{
    std::string::const_iterator start = _it;

    // Read until the second quote
    get();
    while (peek() != '\"' && peek() != '\0')
        get();
    get();

    // Checks if the end was actually a double quote, if it wasn't change the string to unknown.
    bool valid = (*(_it - 1) == '\"');

    return NoodleToken(valid ? NoodleTokenKind::eString : NoodleTokenKind::eUnexpected,
        std::string_view { start + 1, _it - 1 }, // Contents of string without the "quotes"
        linePos(), characterPos());
}

NoodleToken NoodleLexer::createNumber()
{
    std::string::const_iterator start = _it;

    // Read until the last digit
    get();
    while (NoodleLexerUtils::isDigit(peek()))
        get();

    // Detect what type the number is
    // This is almost certainly a hack to get this to work
    // I do not like thi &(*start), &(*_it) trick
    char* longEnd;
    char* floatEnd;

    std::strtol(&(*start), &longEnd, 10);
    std::strtof(&(*start), &floatEnd);

    // If the end pointers don't match the current iterator, somethings wrong with the noodle
    NoodleTokenKind kind = NoodleTokenKind::eUnexpected;

    if (&(*_it) == longEnd)
        kind = NoodleTokenKind::eInteger;
    else if (&(*_it) == floatEnd)
        kind = NoodleTokenKind::eFloat;

    return NoodleToken(kind, std::string_view { start, _it }, linePos(), characterPos());
}

NoodleToken NoodleLexer::createAtom(NoodleTokenKind kind)
{
    return NoodleToken(kind, std::string_view { _it++, _it }, linePos(), characterPos());
}

char NoodleLexer::peek() const
{
    if (_it == _content.end())
        return '\0';
    else
        return *_it;
}

char NoodleLexer::get()
{
    char current = *_it++;

    _characterPos++;
    if (current == '\n') {
        _linePos++;
        _characterPos = 0;
    }

    return current;
}