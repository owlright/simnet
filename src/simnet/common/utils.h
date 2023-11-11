#pragma once
#include <sstream>
#include <string>
#include <vector>
#include <stdint.h>
template <typename T> std::string vectorToString(const std::vector<T>& vec)
{
    std::stringstream tmp;
    for (auto& v : vec) {
        tmp << v << " ";
    }
    return tmp.str();
}

template <class T> inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

void permutation(std::vector<std::vector<int>>& mat, uint32_t seed);