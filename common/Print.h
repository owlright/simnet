#pragma once

#include<iostream>
#include<vector>
template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& array)
{
    os << "[";
    for (auto &elem : array) {
        os << " " << elem;
    }
    os << " ] ";
    return os;
}