#include "graph.h"
#include "algorithms/algorithm.h"
using simnet::algorithms::floyd_warshall;
namespace simnet {

void Graph::init_from(const Mat<double>& adj)
{
    this->adj = adj;
    this->dist = adj;
    floyd_warshall(this->dist);
    auto n = adj.size();
    numVertices = n;
    adjlist.resize(numVertices);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (adj[i][j] > 0 && adj[i][j]!=INFINITY) {
                add_edge(i, j, adj[i][j]);
            }
        }
    }
}

// Initialize graph
Graph::Graph(int vertices)
{
    numVertices = vertices;
    adjlist.resize(numVertices);
}
// Add edges
void Graph::add_edge(int src, int dest, double weight, bool bidirectional)
{
    adjlist[src].push_front(dest);
    if (bidirectional) {
        adjlist[dest].push_front(src);
    }
}

bool Graph::has_edge(int src, int dest) {
    const auto& outneighbours = adjlist[src];
    for (auto& x:outneighbours) {
        if (x==dest) {
            return true;
        }
    }
    return false;
}
}
