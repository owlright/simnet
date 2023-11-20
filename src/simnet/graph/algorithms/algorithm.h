#pragma once
#include "../graph.h"

namespace simnet::algorithms {

void floyd_warshall(Mat<double>& distance);
vector<int> dijistra(const Graph& g, int src, int dest);
Graph takashami_tree(const Graph& g, vector<int> sources, int root);

}