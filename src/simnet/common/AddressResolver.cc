#include "AddressResolver.h"

IntAddress AddressResolver::resolve(const char *s)
{
    std::string modname;
    const char *p = s;
    const char *endp = strchr(p, '\0');
    const char *nextsep = strpbrk(p, "%>(/"); // % for interface like sink%port0
    if (!nextsep)
        nextsep = endp;
    modname.assign(p, nextsep - p);

    // find module
    cModule *mod = getSimulation()->findModuleByPath(modname.c_str());
    if (!mod)
        throw cRuntimeError("AddressResolver: module `%s' not found", modname.c_str());
    if (!mod->par("address").isSet())
        throw cRuntimeError("module `%s' does not have par address", modname.c_str());
    return mod->par("address").intValue();
}