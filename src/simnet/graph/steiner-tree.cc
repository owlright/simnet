#include "algorithms.h"
namespace simnet::algorithms {
Graph takashami_tree(const Graph& g, vector<int> sources, int root)
{
    Graph tree;
    tree.add_node(root);
    auto dist = g.get_dist();
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
            tree.add_edge(path[i], path[i + 1], g.weight(path[i], path[i + 1]));
        }
    }
    return tree;
}

Graph extract_branch_tree(const Graph& tree, const vector<int>& sources, int root, vector<int>* branch_nodes)
{
    // ! make sure tree is actually a directed tree
    Graph t;
    unordered_set<int> visited;
    for (auto& s : sources) {
        auto node = s;
        int edge_start = s;
        while (node != root) {
            if (visited.find(node) != visited.end()) {
                t.add_edge(edge_start, node, tree.distance(edge_start, node));
                break;
            } else {
                visited.insert(node);
            }
            if (tree.indegree(node) > 1) {
                if (branch_nodes)
                    branch_nodes->push_back(node);
                t.add_edge(edge_start, node, tree.distance(edge_start, node));
                edge_start = node;
            }
            node = tree.out_neighbors(node).at(0).first;
        }
        if (node == root) {
            t.add_edge(edge_start, node, tree.distance(edge_start, node));
        }
    }
    return t;
}

vector<int> find_equal_nodes(
    const Graph& g, const Graph& tree, int node, const std::unordered_set<int>& forbiddens, double threshold)
{
    vector<int> equal_nodes;
    std::vector<int> children;
    auto dist = g.get_dist();
    int parent = tree.out_neighbors(node).at(0).first;
    double orig_cost = tree.out_neighbors(node).at(0).second;
    for (auto& [v, w] : tree.in_neighbors(node)) {
        orig_cost += w;
        children.push_back(v);
    }

    for (auto& i : g.get_nodes()) {
        ASSERT(dist);
        if (forbiddens.find(i) == forbiddens.end()) {
            double temp_cost = dist[i][parent];
            for (auto& c : children) {
                temp_cost += dist[c][i];
            }
            if (std::abs(temp_cost - orig_cost) < threshold) {
                equal_nodes.push_back(i);
            }
        }
    }
    return equal_nodes;
}

}
