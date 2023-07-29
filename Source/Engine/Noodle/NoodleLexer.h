#pragma once

#include "NoodleToken.h"
#include "Export.h"

class BLUEMETAL_API NoodleLexerUtils
{
public:
    static bool isSpace(char c);
    static bool isDigit(char c);
    static bool isIdentifierChar(char c);
};

class BLUEMETAL_API NoodleLexer 
{
public:
    explicit NoodleLexer(const std::string& content);

    NoodleToken next();
    int linePos() const;
    int characterPos() const;
private:
    NoodleToken createIdentifier();
    NoodleToken createString();
    NoodleToken createNumber();
    NoodleToken createAtom(NoodleTokenKind kind);
    char peek() const;
    char get();

    const std::string&          _content;
    std::string::const_iterator _it;
    int                         _linePos;
    int                         _characterPos;
};