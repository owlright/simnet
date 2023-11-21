#pragma once
#include "../graph.h"

namespace simnet::algorithms {

void floyd_warshall(Mat<double>& distance);
double dijistra(const Graph& g, int src, int dest, vector<int>* path = nullptr);
Graph takashami_tree(const Graph& g, vector<int> sources, int root);
Graph extract_branch_tree(const Graph& tree, const vector<int>&sources, int root, vector<int>* branch_nodes = nullptr);
}