#pragma once

#include <cstddef>
#include <fstream>
#include <vector>

namespace bl {

template <typename T>
void WriteT(std::ofstream& out, T data)
{
    // static_assert(std::is_fundamental_v<T> == true);
    out.write(reinterpret_cast<char*>(&data), sizeof(T));
}

template <typename T>
void WriteVecT(std::ofstream& out, const std::vector<T>& data)
{
    out.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(T));
}

template <typename T>
T ReadT(std::ifstream& in)
{
    // static_assert(std::is_fundamental_v<T> == true);
    T data;
    in.read(reinterpret_cast<char*>(&data), sizeof(T));
    return data;
}

template <typename T>
std::vector<T> ReadVecT(std::ifstream& in, std::size_t count)
{
    // static_assert(std::is_fundamental_v<T> == true);
    std::vector<T> data(count);
    in.read(reinterpret_cast<char*>(data.data()), sizeof(T) * count);
    return data;
}

}