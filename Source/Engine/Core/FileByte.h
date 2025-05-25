#pragma once

#include "Precompiled.h"

namespace bl
{

template<typename T>
void WriteT(std::ofstream& out, T data)
{
    static_assert(std::is_fundamental_v<T>() == true);
    out.write(reinterpret_cast<char*>(&data), sizeof(T));
}

template<typename T>
T ReadT(std::ifstream& in)
{
    static_assert(std::is_fundamental_v<T>() == true);
    T data;
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
    return data;
}

}