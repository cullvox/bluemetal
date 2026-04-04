#pragma once

#include <string>

class ReferenceCounted
{
    uint64_t _counter;

    friend class Reference;
    void IncrementReferences();
    void DecrementReferences();

public:
    ReferenceCounted();
    ~ReferenceCounted();
};

class Reference
{
    ReferenceCounted* _object;

public:
    Reference();
    Reference(ReferenceCounted& object);
    ~Reference();

    ReferenceCounted* Get();
    bool Valid() const;
}

class ResourceReference
{
    std::string _resource;

};