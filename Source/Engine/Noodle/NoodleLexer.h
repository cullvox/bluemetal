#pragma once

#include "NoodleToken.h"
#include "Export.h"

class BLUEMETAL_API blNoodleLexerUtils
{
public:
    static bool isSpace(char c);
    static bool isDigit(char c);
    static bool isIdentifierChar(char c);
};

class BLUEMETAL_API blNoodleLexer 
{
public:
    explicit blNoodleLexer(const std::string& content);

    blNoodleToken next();
    int linePos() const;
    int characterPos() const;
private:
    blNoodleToken createIdentifier();
    blNoodleToken createString();
    blNoodleToken createNumber();
    blNoodleToken createAtom(blNoodleTokenKind kind);
    char peek();
    char get();

    const std::string&          _content;
    std::string::const_iterator _it;
    int                         _linePos;
    int                         _characterPos;
};