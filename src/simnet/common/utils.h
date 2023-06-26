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
