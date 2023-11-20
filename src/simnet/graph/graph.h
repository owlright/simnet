#pragma once
#include "simnet/common/Defs.h"
#include <list>
#include <vector>
using std::list;
using std::vector;
namespace simnet {
class Graph {
    int numVertices;
    vector<list<int>> adjlist;

public:
    explicit Graph() {};
    explicit Graph(int V);
    void add_edge(int src, int dest, double weight = 1.0, bool bidirectional = false);
    bool has_edge(int src, int dest);
    void init_from(const Mat<double>& adj);
    const Mat<double>& get_dist() const {return dist;};

private:
    Mat<double> dist;
    Mat<double> adj;
};
}
