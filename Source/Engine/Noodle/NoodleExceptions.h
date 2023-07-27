#pragma once

#include "Precompiled.h"

// These would normally be based on blException, but that is in a different 
// module. This modules is supposed to be separated from the others


class NoodleException : public std::exception
{
public:
    NoodleException(const std::string& what)
        : std::exception()
        , _what(what) {}
    virtual const char* what() const noexcept { return _what.c_str(); }
private:
    std::string _what;
};

class NoodleParseException : public NoodleException
{
public:
    NoodleParseException(const std::string& what)
        : NoodleException(what) {}
};

class NoodleInvalidAccessException : public NoodleException
{
public:
    NoodleInvalidAccessException(const std::string& what)
        : NoodleException(what) {}
};