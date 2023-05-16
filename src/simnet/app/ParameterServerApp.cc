#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
    virtual void onNewConnectionArrived(const Packet* const packet) override;
    virtual Packet* createAckPacket(const Packet* const pk) override;

protected:


private:
    GlobalGroupManager* groupManager;
    IntAddress groupAddr{INVALID_ADDRESS};
    int treeIndex{INVALID_ID};
};

Define_Module(ParameterServerApp);

void ParameterServerApp::initialize(int stage)
{
    UnicastEchoApp::initialize(stage);
    if (stage == INITSTAGE_LOCAL) {
        groupManager = findModuleFromTopLevel<GlobalGroupManager>("groupManager", this);
        if (groupManager==nullptr)
            throw cRuntimeError("ParameterServerApp::initialize: groupManager not found!");
    }
    if (stage == INITSTAGE_ASSIGN) {
        groupAddr = groupManager->getGroupAddress(localAddr);
        treeIndex = groupManager->getTreeIndex(localAddr); // TODO for what here?
        if (groupManager->getGroupRootAddress(groupAddr) == localAddr) {
            EV << "(receiver) groupAddr: " << groupAddr <<" localAddr:" << localAddr;
        }
        else
        {
            groupAddr = -1;
        }
    }
}

void ParameterServerApp::onNewConnectionArrived(const Packet* const pk)
{
    UnicastEchoApp::onNewConnectionArrived(pk);
    auto connection = connections.at(pk->getConnectionId());
    connection->bindRemote(pk->getDestAddr(), pk->getLocalPort()); // note here is dest addr not src addr
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pk)
{
    auto packet = UnicastEchoApp::createAckPacket(pk);
    char pkname[40];
    sprintf(pkname, "ACK-%" PRId64 "-to-%" PRId64 "-seq%" PRId64,
            localAddr, pk->getDestAddr(), pk->getSeqNumber());
    packet->setName(pkname);
    return packet;
}
