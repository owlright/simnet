#include "Node.h"

Define_Module(Node);

void Node::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        address = par("address").intValue();
    }
}

void Node::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "address") == 0) {
        address = par("address").intValue();
    }
}

void Node::refreshDisplay() const
{
    if (!getEnvir()->isExpressMode()) {
        char buf[20];
        sprintf(buf, "%" PRId64, address);
        getDisplayString().setTagArg("t", 0, buf);
    }
}
