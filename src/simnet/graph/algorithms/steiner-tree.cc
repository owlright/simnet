#include "algorithm.h"
namespace simnet::algorithms {
Graph takashami_tree(const Graph& g, vector<int> sources, int root)
{
    Graph tree;
    tree.add_node(root);
    auto& dist = g.get_dist();
    for (auto& s : sources) {
        double min_dist = INFINITY;
        int node_in_tree = -1;
        for (auto& n : tree.get_nodes()) {
            if (min_dist > dist[s][n]) {
                min_dist = dist[s][n];
                node_in_tree = n;
            }
        }
        std::vector<int> path;
        dijistra(g, s, node_in_tree, &path);
        for (int i = 0; i < path.size() - 1; i++) {
            tree.add_edge(path[i], path[i + 1], g.get_weight(path[i], path[i + 1]));
        }
    }
    tree.draw("tree");
    return tree;
}
}
