#include <omnetpp.h>
#include "../common/Defs.h"
using namespace omnetpp;

cValue RandomPattern(cComponent *contextComponent, cValue argv[], int argc) {
    auto patten = argv[0].stdstringValue();
    auto rng = argc == 2 ? argv[1].intValue() : 0;
    auto thisHost = contextComponent->getParentModule();
    std::vector<IntAddress> addresses;
    // Loop through all modules and find those that satisfy the criteria
    for (int moduleId = 0; moduleId <= getSimulation()->getLastComponentId(); moduleId++) {
        cModule *module = getSimulation()->getModule(moduleId);
        if (module && (module->getProperties()->get("host") != nullptr)) {
            addresses.push_back(module->par("address"));
        }
    }

    uint32_t dst;
    if (patten == "Uniform") {
        do {
            dst = contextComponent->intrand(addresses.size(), rng);
        }
        while( addresses.at(dst) == thisHost->par("address").intValue() ); // avoid send to itself
    } else {
        throw cRuntimeError("Unknown traffic mode");
    }
    return addresses.at(dst);
}

Define_NED_Function2(RandomPattern,
                     "int RandomPattern(string patten, int rng?)",
                     "random/discrete",
                     "Returns a random destAddress based on a patten");
