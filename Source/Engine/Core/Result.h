#pragma once

#include "Precompiled.h"

namespace bl {

class Result

template<typename TValue = int>
class ResultValue<> {
public:
    Result();
    Result(
    Result(TValue onValue);
    ~Result();

    

private:
    TError _error;
    TValue _value;
};

} // namespace bl

