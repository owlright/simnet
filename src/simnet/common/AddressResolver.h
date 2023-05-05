#pragma once
#include <omnetpp.h>
#include "Defs.h"
using namespace omnetpp;

class AddressResolver
{
public:
    static IntAddress resolve(const char *s);
    static std::vector<IntAddress> resolve(std::vector<std::string> strs);
};