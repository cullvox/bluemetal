#pragma once

#include <stdexcept>

class blNoodleException : public std::exception
{
public:
    blNoodleException(const std::string& what)
        : std::exception() {}
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