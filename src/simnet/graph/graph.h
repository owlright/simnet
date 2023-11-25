#pragma once
#include "simnet/common/Defs.h"
#include <algorithm>
#include <cstdlib>
#include <graphviz/gvc.h>
#include <list>
#include <map>
#include <stack>
#include <unordered_set>
#include <utility>
#include <vector>
using std::list;
using std::make_pair;
using std::map;
using std::pair;
using std::stack;
using std::unordered_set;
using std::vector;
namespace simnet {

class Graph {
public:
    using Edge = pair<int, int>;
    using EdgeWeight = pair<int, double>;

public:
    explicit Graph() {};
    ~Graph()
    {
        if (dist) {
            for (int i = 0; i < max_vertice + 1; i++) {
                free(dist[i]);
            }
            free(dist);
        }
    }
    Graph(const Graph& other)
    {
        if (other.get_dist()) {
            int n = other.get_max_vertice() + 1;
            dist = (double**)malloc(n * sizeof(double*));
            for (int i = 0; i < n; i++) {
                dist[i] = (double*)malloc(n * sizeof(double));
                memcpy(dist[i], other.dist[i], n * sizeof(double));
            }
        }
    }
    void add_edge(int src, int dest, double weight = 1.0, bool bidirectional = false);
    void set_weight(int src, int dest, double weight = 1.0);
    void remove_edge(int src, int dest);
    void add_node(int n);
    void remove_node(int n);
    bool has_node(int n) const;
    bool has_edge(const int& src, const int& dest) const;
    bool has_edge(const Edge&) const;

    void update_dist();
    double distance(int src, int dest) const;
    double weight(int src, int dst) const;
    int get_vertices_number() const { return adjout.size(); }
    int get_max_vertice() const { return max_vertice; }

public:
    const vector<int>& get_nodes() const { return nodes; }
    double** get_dist() const;
    const vector<EdgeWeight>& out_neighbors(int src) const
    {
        if (adjout.find(src) != adjout.end()) {
            return adjout.at(src);
        } else {
            return emptyEdge;
        }
    }
    const vector<EdgeWeight>& in_neighbors(int src) const
    {
        if (adjin.find(src) != adjin.end()) {
            return adjin.at(src);
        } else {
            return emptyEdge;
        }
    }
    vector<int> dfs(int root, bool directionOut=true) const;
    int indegree(int v) const { return adjin.at(v).size(); }
    int outdegree(int v) const { return adjout.at(v).size(); }

public:
    void draw(const char* filename, const char* engine="neato");

private:
    double** dist { nullptr };
    std::vector<int> nodes;
    int max_vertice { -1 };
    map<int, vector<EdgeWeight>> adjout;
    map<int, vector<EdgeWeight>> adjin;
    vector<EdgeWeight> emptyEdge;
};
}
