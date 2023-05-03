#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
    void dealWithDataPacket(Connection *connection, Packet* pk) override;

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
        groupManager = getModuleFromPar<GlobalGroupManager>(par("groupManager"), this);
        if (groupManager==nullptr)
            throw cRuntimeError("ParameterServerApp::initialize: groupManager not found!");
    }
    if (stage == INITSTAGE_ASSIGN) {
        groupAddr = groupManager->getGroupAddress(myAddr);
        treeIndex = groupManager->getTreeIndex(myAddr);
        EV << "groupAddr: " << groupAddr << endl;
    }
}

void ParameterServerApp::dealWithDataPacket(Connection *connection, Packet* pk)
{
    auto packet = new Packet();
    setCommonField(packet);
    packet->setConnectionId(connection->getConnectionId());
    packet->setSeqNumber(pk->getSeqNumber());
    packet->setKind(PacketType::ACK);
    packet->setDestAddr(pk->getDestAddr()); // groupAddress
    packet->setDestPort(pk->getLocalPort());
    if (pk->getECN()) {
        packet->setECE(true);
    }
    connection->sendTo(packet, pk->getDestAddr(), pk->getLocalPort());
}
