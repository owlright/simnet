#pragma once
#include <vector>
#include <sstream>
#include <string>
#include "omnetpp.h"
using namespace omnetpp;

template<typename T>
std::string vectorToString(const std::vector<T>& vec)
{
    std::stringstream tmp;
    for (auto& v:vec) {
        tmp << v << " ";
    }
    return tmp.str();
}

template <class T>
inline void hash_combine(int& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
}