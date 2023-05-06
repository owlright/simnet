#pragma once
#include <omnetpp.h>
using namespace omnetpp;

/**
 * Gets a module in the module tree, given by its absolute or relative path
 * defined by 'par' parameter.
 * Returns the pointer to a module of type T or throws an error if module not found
 * or type mismatch.
 */
template<typename T>
T *getModuleFromPar(cPar& par, const cModule *from)
{
    const char *path = par;
    cModule *mod = from->findModuleByPath(path);
    if (!mod) {
        throw cRuntimeError("Module not found on path '%s' defined by par '%s'", path, par.getFullPath().c_str());
    }
    T *m = dynamic_cast<T *>(mod);
    if (!m)
        throw cRuntimeError("Module can not cast to '%s' on path '%s' defined by par '%s'", opp_typename(typeid(T)), path, par.getFullPath().c_str());
    return m;
}
