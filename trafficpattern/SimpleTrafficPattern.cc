#include <omnetpp.h>
using namespace omnetpp;

// ! the function is right, but the topology initialize after ned functions
// ! so topo do not work right!
cValue RandomPattern(cComponent *contextComponent, cValue argv[], int argc) {
    auto patten = argv[0].stdstringValue();
    auto rng = argc == 2 ? argv[1].intValue() : 0;
    auto topo = new cTopology("topo");
    topo->extractByProperty("host"); // get all host nodes, no switches
    auto hostNumber = topo->getNumNodes();
//    std::cout << "find " << hostNumber << " nodes" << endl;
    std::vector<int> nodeMap(hostNumber);
    for (int i = 0; i < topo->getNumNodes(); i++) {
        int address = topo->getNode(i)->getModule()->par("address");
        nodeMap[i] = address;
    }
    auto thisHost = contextComponent->getParentModule();
//    auto pod = host->getParentModule();

//    static int podNumber = pod->getVectorSize();
//    static int hostNumber = host->getVectorSize();
//    int podId = pod->getIndex();
//    int hostId = host->getIndex();
//    int podDst, hostDst;
    int dst = -1;
    if (patten == "Uniform") {

        do {
            dst = contextComponent->intrand(hostNumber, rng);
        }
        while( nodeMap[dst] == thisHost->par("address").intValue() );
//        std::cout << thisHost->par("address").intValue() << " " << nodeMap[dst] << endl;

//        if (dst >= podId * hostNumber + hostId)
//            dst++;
        // podDst = dst / hostNumber;
        // hostDst = dst % hostNumber;
//    } else if (patten == "Adversarial") {
//        podDst = (podId + 1) % podNumber;
//        hostDst = contextComponent->intrand(hostNumber, rng);
    } else {
        throw cRuntimeError("Unknown traffic mode");
    }
    delete topo;
    return nodeMap[dst];
//    return std::string("pods[") + std::to_string(podDst) + "].hosts[" + std::to_string(hostDst) + "]";
}

Define_NED_Function2(RandomPattern,
                     "int RandomPattern(string patten, int rng?)",
                     "random/discrete",
                     "Returns a random destAddress based on a patten");
