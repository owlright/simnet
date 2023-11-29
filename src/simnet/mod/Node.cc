#include "Node.h"

Define_Module(Node);

void Node::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        address = par("address");
    }
}
void Node::handleMessage(cMessage* msg) { throw cRuntimeError("This is a base class, you shouldn't use it directly."); }

void Node::handleParameterChange(const char* parameterName)
{
    // ! if stage == INITSTAGE_ASSIGN, this may happen
    if (strcmp(parameterName, "address") == 0) {
        address = par("address");
        std::vector<const char*> notifyapps { "apps", "workers", "pses" };
        for (auto name : notifyapps) {
            if (hasSubmoduleVector(name)) {
                auto apps = getSubmoduleArray(name);
                for (auto& app : apps) {
                    app->par("address") = address;
                }
            }
        }
        auto routing = getSubmodule("routing");
        routing->par("address") = address;
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
        numFlows = par("numFlows");
        loadMode = load > 0.0 && numFlows != 0;
        flowInterval = par("flowInterval");
        bandwidth = par("bandwidth");
        if (bandwidth <= 0) { // get bandwidth value automatically
            ASSERT(getGateNames().size() == 1);
            auto gateName = getGateNames()[0].c_str();
            bandwidth = check_and_cast<cDatarateChannel*>(gateHalf(gateName, cGate::Type::OUTPUT, 0)->getChannel())
                            ->getDatarate();
            par("bandwidth") = bandwidth; // ! apps will read this value
        }
        metricCollector = findModuleFromTopLevel<GlobalMetricCollector>("metricCollector", this);
        if (!metricCollector)
            throw cRuntimeError("Can't find metricCollector");
    } else if (stage == INITSTAGE_ACCEPT) { // ! after jobs are assigned
        if (getSubmoduleVectorSize("workers") > 0 || getSubmoduleVectorSize("pses") > 0) {
            loadMode = false;
        }
        if (loadMode) {
            flowSizeMean = par("flowSizeMean");
            ASSERT(flowSizeMean > 0);
            flowInterval = (flowSizeMean * 8) / (bandwidth * load); // load cannot be zero
            tpManager
                = findModuleFromTopLevel<TrafficPatternManager>("tpManager", this); // we only need this in load mode
            if (tpManager == nullptr)
                throw cRuntimeError("In loadMode, you must set the tpManager");
            newFlowTimer = new cMessage("newFlow");
            scheduleAfter(exponential(flowInterval), newFlowTimer);
            metricCollector->registerFlowMetric(address, numFlows);
        }
    }
}

void HostNode::handleMessage(cMessage* msg)
{
    ASSERT(msg->isSelfMessage());
    if (msg == newFlowTimer) {
        flowCount++;
        startNewFlow();
        if (flowCount != numFlows)
            scheduleAfter(exponential(flowInterval), newFlowTimer);
    }
}

void HostNode::finish()
{
    // ! Note apps[0] is ParameterServerApp
    // for (auto i = 1; i < getSubmoduleVectorSize("apps"); i++) {
    //     auto app = getSubmodule("apps", i);
    //     app->deleteModule();
    // }
    // for (auto i = 0; i < getSubmoduleVectorSize("workers"); i++) {
    //     auto app = getSubmodule("workers", i);
    //     app->deleteModule();
    // }
    // for (auto i = 0; i < getSubmoduleVectorSize("pses"); i++) {
    //     auto app = getSubmodule("pses", i);
    //     app->deleteModule();
    // }
    EV_DEBUG << address << " generate " << getSubmoduleVectorSize("apps") << " FlowApps." << endl;
    EV_DEBUG << address << " generate " << flowCount << " flows." << endl;
}

FlowApp* HostNode::createCongApp()
{
    auto appExistSize = getSubmoduleVectorSize("apps");
    if (appExistSize == 0)
        throw cRuntimeError("apps[0] must be EchoApp.");
    EV_TRACE << "create CongApp with port " << 1001 + appExistSize << endl;
    setSubmoduleVectorSize("apps", appExistSize + 1);
    auto appType = "simnet.app.FlowApp";
    cModule* app = cModuleType::get(appType)->create("apps", this, appExistSize);
    app->par("address") = address;
    app->par("port") = 1000 + appExistSize;
    app->par("destAddress") = generateDestAddr();
    app->par("flowStartTime") = simTime().dbl();
    app->finalizeParameters();
    app->buildInside();

    app->scheduleStart(simTime());
    auto inGate = app->gate("in");
    auto outGate = app->gate("out");
    auto at = getSubmodule("at");
    at->setGateSize("localIn", at->gateSize("localIn") + 1);
    at->setGateSize("localOut", at->gateSize("localOut") + 1);
    at->gate("localOut", at->gateSize("localIn") - 1)->connectTo(inGate);
    outGate->connectTo(at->gate("localIn", at->gateSize("localOut") - 1));
    app->callInitialize();
    return check_and_cast<FlowApp*>(app);
}

void HostNode::startNewFlow()
{
    bool foundIdleApp = false;
    for (auto& app : unicastSenders) {
        if (app->getAppState() == Finished) {
            if (app->getAppState() == Finished)
                app->setDestAddr(generateDestAddr()); // reassign a destAddr, destPort is not changed
            app->scheduleNextFlowAt(simTime());
            foundIdleApp = true;
            break;
        }
    }
    if (!foundIdleApp) { // we need to create a new app
        auto app = createCongApp();
        unicastSenders.push_back(app);
    }
}

IntAddress HostNode::generateDestAddr()
{
    ASSERT(tpManager);
    auto destAddr = tpManager->getDestAddr(address);
    return destAddr;
}

HostNode::~HostNode() { cancelAndDelete(newFlowTimer); }

Define_Module(SwitchNode);
