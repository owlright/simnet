#include "Node.h"

Define_Module(Node);

void Node::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        address = par("address").intValue();
        isHost = getProperties()->get("host") != nullptr;
    }
    else if (stage == INITSTAGE_LAST) {
        if (isHost) {
            auto apps = getSubmoduleArray("apps");
            std::vector<const char*> parNames{"port", "destAddress", "destPort"};
            for (auto app:apps) {
                for (auto parname: parNames) {
                    if (app->hasPar(parname)) {
                        EV_TRACE << app->par(parname) << " ";
                    }
                }
                EV_TRACE << endl;
                if (app->hasPar("destAddress") &&
                        app->par("destAddress").intValue() == INVALID_ADDRESS) {
                    EV_WARN << "node " << address << " has idle sender " << app->getClassAndFullName() << endl;
                    app->callFinish();
                    app->deleteModule();
                }
            }

        }

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
