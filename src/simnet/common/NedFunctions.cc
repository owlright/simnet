#include <omnetpp.h>
#include "simnet/common/Defs.h"
using namespace omnetpp;

cNEDValue nedf_absPath(cComponent *context, cNEDValue argv[], int argc)
{
    if (argc != 1)
        throw cRuntimeError("absPath(): must be one argument instead of %d argument(s)", argc);
    const char *path = argv[0].stringValue();
    switch (*path) {
        case '.':
            return context->getFullPath() + path;

        case '^':
            return context->getFullPath() + '.' + path;

        default:
            return argv[0];
    }
}

Define_NED_Function2(nedf_absPath,
        "string absPath(string modulePath)",
        "string",
        "Returns absolute path of given module"
        );