#include "Node.h"


Define_Module(Node);

void Node::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        address = par("address").intValue();
    }

}
void Node::handleMessage(cMessage * msg)
{
    throw cRuntimeError("This is a base class, you shouldn't use it directly.");
}

void Node::handleParameterChange(const char *parameterName)
{
    if (strcmp(parameterName, "address") == 0) {
        address = par("address");
        // std::cout << getClassAndFullPath() << std::endl;
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


Define_Module(HostNode);

void HostNode::initialize(int stage)
{
    Node::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        load = par("load");

        flowInterval = par("flowInterval");
        bandwidth = par("bandwidth");
        if (bandwidth <= 0) { // get bandwidth value automatically
            bandwidth = check_and_cast<cDatarateChannel *>(
                                        gateHalf("port", cGate::Type::OUTPUT, 0)
                                        ->getChannel())->getDatarate();
            par("bandwidth") = bandwidth; // ! apps will read this value
        }
        if (load > 0.0) {
            flowSizeMean = par("flowSizeMean");
            ASSERT(flowSizeMean > 0);
            flowInterval = flowSizeMean / (bandwidth * load); // load cannot be zero
        }
        trafficPattern = par("trafficPattern");
        newFlowTimer = new cMessage("newFlow");
    }
    else if (stage == INITSTAGE_ASSIGN) {

    }
}

void HostNode::handleMessage(cMessage* msg)
{
    if (msg == newFlowTimer) {
        startNewFlow();
    }
}


opp_component_ptr<UnicastSenderApp>
HostNode::createUnicastSenderApp()
{
    return opp_component_ptr<UnicastSenderApp>();
}

void HostNode::startNewFlow()
{
    auto apps = getSubmoduleArray("apps");
    bool foundIdleApp = false;
    for (auto mod:apps) {
        if (strcmp(mod->getClassName(), "UnicastSenderApp") == 0) {
            auto app = check_and_cast<UnicastSenderApp*>(mod);
            if (app->getAppState() == Idle || app->getAppState() == Finished) {
                app->par("flowStartTime") = (simTime() + exponential(flowInterval)).dbl();
                foundIdleApp = true;
            }
        }
        if (foundIdleApp == true)
            break;
    }
    if (!foundIdleApp) { // we need to create a new app
        auto app = createUnicastSenderApp();
        app->par("flowStartTime") = (simTime() + exponential(flowInterval)).dbl();
    }
}

HostNode::~HostNode()
{
    cancelAndDelete(newFlowTimer);
}

Define_Module(SwitchNode);
