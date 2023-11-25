#pragma once
#include "graph.h"

namespace simnet::algorithms {
using Path = vector<int>;
void floyd_warshall(Mat<double>& distance, int N);
void floyd_warshall(double** distance, int N);
double dijistra(const Graph& g, int src, int dest, vector<int>* path = nullptr);
Graph takashami_tree(const Graph& g, vector<int> sources, int root);
Graph extract_branch_tree(const Graph& tree, const vector<int>& sources, int root, vector<int>* branch_nodes = nullptr);
vector<double> yenksp(Graph& g, int src, int dest, int K, vector<Path>& A);
vector<int> find_equal_nodes(const Graph& g, const Graph& tree, int node,
    const std::unordered_set<int>& forbiddens = std::unordered_set<int>(), double threshold = 0.0);
}
