#include "algorithms.h"
#include "simnet/common/utils.h"
#include <queue>
namespace simnet::algorithms {
Graph takashami_tree(const Graph& g, vector<int> sources, int root)
{
    Graph tree;
    tree.add_node(root);
    std::unordered_set<int> visited;
    for (auto i = 0; i < sources.size(); i++) {
        double min_dist = INFINITY;
        int node_in_tree = -1;
        int min_src = -1;
        for (auto& s : sources) {
            if (visited.find(s) == visited.end()) {
                for (auto& n : tree.get_nodes()) {
                    auto curr_dist = g.distance(s, n);
                    if (visited.find(n) == visited.end() && min_dist > curr_dist) {
                        min_dist = curr_dist;
                        node_in_tree = n;
                        min_src = s;
                    }
                }
            }
        }
        ASSERT(min_src != -1);
        visited.insert(min_src);
        std::vector<int> path;
        dijistra(g, min_src, node_in_tree, &path);
        if (path.size() < 2) {
            throw cRuntimeError("graph is not connected");
        }
        for (int i = 0; i < path.size() - 1; i++) {
            tree.add_edge(path[i], path[i + 1], g.weight(path[i], path[i + 1]));
        }
    }

    if (!tree.is_tree()) {
        tree.draw("wrong.png");
    }
    ASSERT(tree.is_tree());
    return tree;
}

vector<Graph> takashami_trees(const Graph& g, vector<int> sources, int root, const unordered_set<int>& forbiddens,
    vector<map<int, vector<int>>>* equal_branch_nodes)
{
    vector<Graph> trees {};

    std::unordered_set<Graph, Graph::Hash> visitedBranchTrees;
    std::queue<Graph> waited;
    waited.push(takashami_tree(g, sources, root));
    ASSERT(g.is_connected());
    while (!waited.empty()) {
        auto t = waited.front();
        waited.pop();
        vector<int> branch_nodes;
        auto branch_tree = extract_branch_tree(t, sources, root, &branch_nodes);
        if (visitedBranchTrees.find(branch_tree) == visitedBranchTrees.end()) {
            visitedBranchTrees.insert(branch_tree);
            trees.push_back(t);
            if (equal_branch_nodes) {
                equal_branch_nodes->push_back(map<int, vector<int>>());
            }
            unordered_set<int> forbiddenmore(forbiddens.begin(), forbiddens.end());
            for (auto& b : branch_nodes) {
                forbiddenmore.insert(b);
            }
            for (auto& b : branch_nodes) {
                vector<int> equals = find_equal_nodes(g, branch_tree, b, forbiddenmore);
                if (equal_branch_nodes) {
                    equal_branch_nodes->back()[b] = equals;
                }

                auto gcopy = g;
                gcopy.remove_node(b);
                for (auto n : equals) {
                    gcopy.remove_node(n);
                }
                if (gcopy.is_connected()) {
                    std::vector<int> newBranchNodes;
                    auto newt = takashami_tree(gcopy, sources, root);
                    auto newBranchTree = extract_branch_tree(newt, sources, root, &newBranchNodes);
                    if (visitedBranchTrees.find(newBranchTree) == visitedBranchTrees.end()) {
                        waited.push(newt);
                    }
                } else {
                    continue;
                }
            }
        }
    }
    return trees;
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
        if (forbiddens.find(i) == forbiddens.end() && i != node) {
            double temp_cost = dist[i][parent];
            for (auto& c : children) {
                temp_cost += dist[c][i];
            }
            if (std::abs(temp_cost - orig_cost) <= threshold) {
                equal_nodes.push_back(i);
            }
        }
    }
    return equal_nodes;
}

}
