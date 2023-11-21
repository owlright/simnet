#pragma once
#include "simnet/common/Defs.h"
#include <list>
#include <map>
#include <utility>
#include <vector>
using std::list;
using std::make_pair;
using std::map;
using std::pair;
using std::vector;
#include <graphviz/gvc.h>
namespace simnet {
class Graph {
    using Edge = pair<int, int>;
    using EdgeWeight = pair<int, double>;
    map<int, vector<EdgeWeight>> adj;
    map<int, vector<EdgeWeight>> adjreverse;

public:
    explicit Graph() {};
    void add_edge(int src, int dest, double weight = 1.0, bool bidirectional = false);
    void add_node(int n);
    bool has_node(int n);
    bool has_edge(const int& src, const int& dest);
    bool has_edge(const Edge&);
    void update_dist();
    const vector<int>& get_nodes() const { return nodes; }
    const Mat<double>& get_dist() const;
    const vector<EdgeWeight>& get_out_neighbors(int src) const { return adj.at(src); }
    const vector<EdgeWeight>& get_in_neighbors(int src) const { return adjreverse.at(src); }
    int get_vertices_number() const { return adj.size(); }
    double get_weight(int src, int dst) const;

public:
    void draw(const char* filename);

private:
    Mat<double> dist;
    std::vector<int> nodes;
};
}
