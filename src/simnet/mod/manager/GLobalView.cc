#include "GlobalView.h"
#include <algorithm>
Define_Module(GlobalView);

Dict<int> GlobalView::primMST(const vector<int>& S, const vector<int>& mstnodes, const Mat<double> &oddist)
{
    auto P = Dict<int>();
    auto r = mstnodes[0]; // ! mstnodes[0] must be target
    auto Tlen = mstnodes.size();
    auto in_mst_set = vector<bool>(Tlen, false);
    auto parent = vector<int>(Tlen, -1);
    auto dist = vector<double>(Tlen, INFINITY);
    dist[0] = 0;

    for (auto nouse = 0; nouse < Tlen; nouse++) {
        double d = INFINITY;
        size_t closest_index = 0;
        int closest_node = -1;
        // * get the closet node to the tree
        for (auto i = 0; i < Tlen; i++) {
            if (!in_mst_set[i] && dist[i] < d) {
                d = dist[i];
                closest_index = i;
                closest_node = mstnodes[i];
            }
        }
        in_mst_set[closest_index] = true;
        if (closest_node != r) {
            P[closest_node] = mstnodes[parent[closest_index]];
        }
        // * update the rest nodes' distance to T
        for (auto i = 0; i < Tlen; i++) {
            auto v = mstnodes[i];
            if (!in_mst_set[i] && oddist[v][closest_node] < dist[i]) {
                dist[i] = oddist[v][closest_node];
                parent[i] = closest_index;
            }
        }
    }
    for (auto i = 0; i < Tlen; i++) {
        auto s = S[i];
        auto d = INFINITY;
        for (auto j = 0; j < Tlen; j++) {
            if (oddist[s][mstnodes[j]] < d) {
                d = oddist[s][mstnodes[j]];
                P[s] = mstnodes[j];
            }
        }

    }
    return P;
}

void GlobalView::initialize(int stage)
{
    if (stage == INITSTAGE_LOCAL) {
        EV << "network initialization." << endl;
        topo = new cTopology("topo");
        topo->extractByProperty("node");
        // int N = topo->getNumNodes();
        // topoDist.resize(N, vector<double>(N, 0.0));
        // costAdj.resize(N, vector<double>(N, INFINITY));
        // for (int i = 0; i < topo->getNumNodes(); i++) {
        //     auto u = topo->getNode(i);
        //     topo->calculateWeightedSingleShortestPathsTo(u); // all paths v -> u
        //     for (int j = i + 1; j < topo->getNumNodes(); j++) {
        //         auto v = topo->getNode(j);
        //         topoDist[j][i] = v->getDistanceToTarget();
        //         topoDist[i][j] = topoDist[j][i];
        //         if (v->getPath(0)->getRemoteNode() == u) { // ! if v is the adj node of u
        //             costAdj[j][i] = 1.0;
        //             costAdj[i][j] = 1.0;
        //         }
        //         // std::cout << i << " "
        //         //           << j << " "
        //         //           << topoDist[i][j] << endl;
        //         // std::cout << topo->getNode(i)->getModule()->getClassAndFullPath() << " "
        //         //           << topo->getNode(j)->getModule()->getClassAndFullPath() << " "
        //         //           << topoDist[i][j] << endl;
        //     }
        // }
        // EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        collectNodes(topo);
        // auto S = vector<int>{8, 9, 10, 11};
        // auto mstnodes = vector<int>{26, 2, 4, 7};
        // auto tmp = primMST(S, mstnodes, topoDist);
        // std::cout << endl;
        // std::cout << "sources: ";
        // for (const auto& a: S)
        //     std::cout << nodeId2Addr[a] << " ";
        // std::cout << endl;
        // std::cout << nodeId2Addr[mstnodes[0]] << endl;
        // for (const auto& [a, b]:tmp) {
        //     std::cout << nodeId2Addr[a] << " " << nodeId2Addr[b] << endl;
        // }
        // std::cout << endl;
        // auto print = [](const std::vector<double>& n) { std::cout << n << endl; };
        // std::for_each(costAdj.cbegin(), costAdj.cend(), print);
    }
}

void GlobalView::collectNodes(cTopology *topo)
{
    nodeId2Addr.reserve(topo->getNumNodes());
    std::unordered_set<IntAddress> used;
    for (int i = 0; i < topo->getNumNodes(); i++)
    {
        auto node = topo->getNode(i);
        auto nodeMod = node->getModule();
        // std::cout << i << " " << nodeMod->getId() << " "
        //                       << nodeMod->getClassAndFullPath() << endl;
        IntAddress address = nodeMod->par("address");
        if (address == -1) { // automanually set it by object id which is ensured unique
            address = nodeMod->getId();
            nodeMod->par("address") = address;
        }
        if (used.find(address) != used.end()){
            throw cRuntimeError("GlobalView::collectNodes: duplicate address %" PRId64, address);
        }
        used.insert(address);
        auto isHost = nodeMod->getProperties()->get("host") != nullptr;
        if (isHost) {
            hostNodes.push_back(i);
        }
        nodeId2Addr[i] = address;
        addr2NodeId[address] = i;
        EV_DEBUG << "node: " << i << " address: " << address << endl;
    }
    EV_INFO << "There are " << hostNodes.size() << " hosts and " << topo->getNumNodes() - hostNodes.size() << " switches" << endl;
}

