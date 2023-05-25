#include "UnicastEchoApp.h"
#include "simnet/common/ModuleAccess.h"
#include "simnet/mod/manager/GlobalGroupManager.h"

class ParameterServerApp : public UnicastEchoApp
{
protected:
    void initialize(int stage) override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void onNewConnectionArrived(IdNumber connId, const Packet* const packet) override;
    virtual void connectionDataArrived(Connection *connection, cMessage *msg) override;
    virtual Packet* createAckPacket(const Packet* const pk) override;

protected:
    std::unordered_map<SeqNumber, int> aggCounters;

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
            EV << "(receiver) groupAddr: " << groupAddr <<" localAddr:" << localAddr << endl;
        }
        else
        {
            groupAddr = -1;
        }
    }
}

void ParameterServerApp::handleMessage(cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    auto connectionId = pk->getDestAddr(); // ! use groupAddr as connectionId
    auto it = connections.find(connectionId);
    if (it == connections.end()) {
        onNewConnectionArrived(connectionId, pk);
    }
    connections.at(connectionId)->processMessage(pk);
}

void ParameterServerApp::onNewConnectionArrived(IdNumber connId, const Packet* const pk)
{
    connections[connId] = createConnection(connId);
    auto connection = connections.at(connId);
    connection->bindRemote(connId, pk->getLocalPort());
}

void ParameterServerApp::connectionDataArrived(Connection *connection, cMessage *msg)
{
    auto pk = check_and_cast<Packet*>(msg);
    ASSERT(pk->getKind() == PacketType::DATA);
    ASSERT(pk->getDestAddr() == connection->getConnectionId());
    auto seq = pk->getSeqNumber();
    if (aggCounters.find(seq) == aggCounters.end())
       aggCounters[seq] = 0;
    aggCounters.at(seq) += pk->getAggCounter();
    EV_DEBUG << "Seq " << seq << " aggregated " << aggCounters.at(seq) << endl;
    if (aggCounters.at(seq) == pk->getAggNumber())
    {
        auto packet = createAckPacket(pk);
        connection->send(packet);
        aggCounters.erase(seq);
        EV_DEBUG << "Seq " << seq << " finished." << endl;
        // TODO destroy this connection
    }
    delete pk;
}

Packet* ParameterServerApp::createAckPacket(const Packet* const pk)
{
    auto packet = UnicastEchoApp::createAckPacket(pk);
    char pkname[40];
    sprintf(pkname, "ACK-%lld-to-%lld-seq%lld",
            localAddr, pk->getDestAddr(), pk->getSeqNumber());
    packet->setName(pkname);
    return packet;
}
