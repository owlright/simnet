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
#include <queue>

using std::list;
using std::make_pair;
using std::map;
using std::pair;
using std::stack;
using std::unordered_set;
using std::vector;
using std::queue;

namespace simnet {

class Graph {
public:
    using Edge = pair<int, int>;
    using EdgeWeight = pair<int, double>;

public:
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
    /**
     * graph algorithms
     */
    vector<int> dfs(int root, bool directionOut = true) const;
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

    int indegree(int v) const
    {
        ASSERT(adjin.find(v) != adjin.end());
        return adjin.at(v).size();
    }
    int outdegree(int v) const
    {
        ASSERT(adjout.find(v) != adjout.end());
        return adjout.at(v).size();
    }

public:
    void draw(const char* filename, const char* engine = "neato");

public:
    explicit Graph();
    ~Graph();
    Graph(const Graph& other);
    // assign constructor;
    // Graph a, b
    // a = b will call this;
    Graph& operator=(const Graph& other);
    // Move constructor
    Graph(Graph&& other);
    // Move assignment operator
    // auto a = getNetworkCopy() will call this
    Graph& operator=(Graph&& other);

private:
    double** dist { nullptr };
    std::vector<int> nodes;
    int max_vertice { -1 };
    map<int, vector<EdgeWeight>> adjout;
    map<int, vector<EdgeWeight>> adjin;
    vector<EdgeWeight> emptyEdge;
};
}
