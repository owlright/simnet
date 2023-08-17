#pragma once

#include<iostream>
#include<vector>
#include<unordered_map>

template<class T>
std::ostream& operator<<(std::ostream& os, const std::vector<T>& array)
{
    os << "[";
    for (auto i = 0; i < array.size(); i++) {
        if (i != 0)
            os << ",";
        os << array[i];
    }
    os << "]";
    return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::unordered_map<K, V>& map) {
    for (const auto& [key, value] : map) {
        os << key << ": " << value << ", ";
    }
    return os;
}

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& map) {
    for (const auto& [key, value] : map) {
        os << key << ": " << value << ", ";
    }
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::unordered_set<T>& array)
{
    os << "[";
    int count = 0;
    for (const auto& i: array) {
        if (count != 0)
            os << ",";
        os << i;
        count++;
    }
    os << "]";
    return os;
}

template<class T>
std::ostream& operator<<(std::ostream& os, const std::set<T>& array)
{
    os << "[";
    int count = 0;
    for (const auto& i: array) {
        if (count != 0)
            os << ",";
        os << i;
        count++;
    }
    os << "]";
    return os;
}

namespace style {
enum {
    reset     = 0,
    bold      = 1,
    dim       = 2,
    italic    = 3,
    underline = 4,
    blink     = 5,
    rblink    = 6,
    reversed  = 7,
    conceal   = 8,
    crossed   = 9
};
}

namespace fg {
enum {
    black   = 30,
    red     = 31,
    green   = 32,
    yellow  = 33,
    blue    = 34,
    magenta = 35,
    cyan    = 36,
    gray    = 37,
    reset   = 39
};
}

namespace bg {
enum {
    black   = 40,
    red     = 41,
    green   = 42,
    yellow  = 43,
    blue    = 44,
    magenta = 45,
    cyan    = 46,
    gray    = 47,
    reset   = 49
};
}


namespace fgb {
enum {
    black   = 90,
    red     = 91,
    green   = 92,
    yellow  = 93,
    blue    = 94,
    magenta = 95,
    cyan    = 96,
    gray    = 97
};
}
namespace bgB {
enum {
    black   = 100,
    red     = 101,
    green   = 102,
    yellow  = 103,
    blue    = 104,
    magenta = 105,
    cyan    = 106,
    gray    = 107
};
}

#define ESC "\033["
#define ENDC ESC "0m"
#define COLOR(name) ESC << std::to_string(name) << "m"
#define END ENDC << endl
