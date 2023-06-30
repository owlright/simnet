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
        loadMode = load > 0.0;
        flowInterval = par("flowInterval");
        bandwidth = par("bandwidth");
        if (bandwidth <= 0) { // get bandwidth value automatically
            bandwidth = check_and_cast<cDatarateChannel *>(
                                        gateHalf("port", cGate::Type::OUTPUT, 0)
                                        ->getChannel())->getDatarate();
            par("bandwidth") = bandwidth; // ! apps will read this value
        }
        if (loadMode) {
            flowSizeMean = par("flowSizeMean");
            ASSERT(flowSizeMean > 0);
            flowInterval = flowSizeMean / (bandwidth * load); // load cannot be zero
            tpManager = findModuleFromTopLevel<TrafficPatternManager>("tpManager", this); // we only need this in load mode
            if (tpManager == nullptr)
                throw cRuntimeError("In loadMode, you must set the tpManager");
            newFlowTimer = new cMessage("newFlow");
            scheduleAfter(exponential(flowInterval), newFlowTimer);
        }
        // * collect manually setup unicast apps if there is any
        auto apps = getSubmoduleArray("apps");
        for (auto mod:apps) {
            if (strcmp(mod->getClassName(), "UnicastSenderApp") == 0) {
                auto app = check_and_cast<UnicastSenderApp*>(mod);
                unicastSenders.push_back(app);
            }
        }

    }
    else if (stage == INITSTAGE_ASSIGN) {
        for (auto app:unicastSenders) {
            if (app->getDestAddr() == INVALID_ADDRESS) {
                app->setDestAddr(generateDestAddr());
            }
        }
    }
}

void HostNode::handleMessage(cMessage* msg)
{
    ASSERT(msg->isSelfMessage());
    if (msg == newFlowTimer) {
        startNewFlow();
        scheduleAfter(exponential(flowInterval), newFlowTimer);
    }
}


opp_component_ptr<UnicastSenderApp>
HostNode::createUnicastSenderApp()
{
    return opp_component_ptr<UnicastSenderApp>();
}

void HostNode::startNewFlow()
{
    bool foundIdleApp = false;
    for (auto& app:unicastSenders) {
        if (app->getAppState() == Idle || app->getAppState() == Finished) {
            if (app->getAppState() == Finished)
                app->setDestAddr(generateDestAddr());
            app->scheduleNextFlowAfter((simTime() + exponential(flowInterval)).dbl());
            foundIdleApp = true;
            break;
        }
    }
    if (!foundIdleApp) { // we need to create a new app
        auto app = createUnicastSenderApp();
        unicastSenders.push_back(app);
    }
}

HostNode::~HostNode()
{
    cancelAndDelete(newFlowTimer);
}

Define_Module(SwitchNode);
