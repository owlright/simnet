#include "GlobalManager.h"
#include "simnet/common/ModuleAccess.h"

IntAddress GlobalManager::getAddr(int nodeId) const
{
    IntAddress addr = nodeId2Addr.at(nodeId);
    return addr;
}

IntAddress GlobalManager::getAddr(cModule *mod) const
{
    IntAddress addr = mod->par("address");
    return addr;
}

int GlobalManager::getNodeId(IntAddress addr) const
{
    int nodeId = addr2NodeId.at(addr);
    return nodeId;
}

cModule *GlobalManager::getMod(IntAddress addr) const
{
    int nodeId = getNodeId(addr);
    return topo->getNode(nodeId)->getModule();
}

cTopology::Node *GlobalManager::getNode(IntAddress addr) const
{
    int nodeId = getNodeId(addr);
    return topo->getNode(nodeId);
}

void GlobalManager::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        globalView = findModuleFromTopLevel<GlobalView>("globalView", this);
        if (globalView == nullptr)
            throw cRuntimeError("Fail to get globalView");
    }
    else if (stage == INITSTAGE_ASSIGN) {
        topo = globalView->getGlobalTopo();
        hostIds = globalView->gethostIds();
        nodeId2Addr = globalView->getNodeIdAddrMap();
        addr2NodeId = globalView->getAddrNodeIdMap();
    }
    else if (stage == INITSTAGE_LAST) {
        ASSERT(topo);
    }
}

