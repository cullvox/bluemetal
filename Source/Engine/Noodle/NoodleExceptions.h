#pragma once

#include "Noodle/Precompiled.hpp"

// These would normally be based on blException, but that is in a different 
// module. This modules is supposed to be separated from the others


class blNoodleException : public std::exception
{
public:
    blNoodleException(const std::string& what)
        : std::exception()
        , _what(what) {}
    virtual const char* what() const noexcept { return _what.c_str(); }
private:
    std::string _what;
};

class blNoodleParseException : public blNoodleException
{
public:
    blNoodleParseException(const std::string& what)
        : blNoodleException(what) {}
};

class blNoodleInvalidAccessException : public blNoodleException
{
public:
    blNoodleInvalidAccessException(const std::string& what)
        : blNoodleException(what) {}
};