#include "graph.h"
#include "algorithms/algorithm.h"
using simnet::algorithms::floyd_warshall;
namespace simnet {
bool is_in_vector(const int& x, const vector<int> vec)
{
    for (auto& i : vec) {
        if (i == x) {
            return true;
        }
    }
    return false;
}
// Add edges
void Graph::add_edge(int src, int dest, double weight, bool bidirectional)
{
    if (!is_in_vector(src, nodes))
        nodes.push_back(src);
    if (!is_in_vector(dest, nodes))
        nodes.push_back(dest);
    if (!has_edge(src, dest)) {
        adj[src].push_back(make_pair(dest, weight));
        if (bidirectional) {
            adj[dest].push_back(make_pair(src, weight));
        }
    } else {
        auto outneighbours = adj.at(src);
        for (auto& x : outneighbours) {
            if (x.first == dest) {
                x.second = weight;
            }
        }
    }
}

void Graph::add_node(int n) {
    if (!is_in_vector(n, nodes)) {
        nodes.push_back(n);
    }
 }

bool Graph::has_edge(int src, int dest)
{
    if (adj.find(src) == adj.end()) {
        return false;
    } else {
        const auto& outneighbours = adj.at(src);
        for (auto& x : outneighbours) {
            if (x.first == dest) {
                return true;
            }
        }
    }
    return false;
}

void Graph::update_dist()
{
    dist.resize(adj.size(), vector<double>(adj.size(), INFINITY));
    for (auto& uv : adj) {
        auto& u = uv.first;
        auto& neighbours = uv.second;
        for (auto& [v, w] : neighbours) {
            dist[u][v] = w;
        }
    }
    floyd_warshall(dist);
}

const Mat<double>& Graph::get_dist() const
{
    if (dist.empty()) {
        throw cRuntimeError("must call update_dist() first!");
    } else {
        return dist;
    }
}

double Graph::get_weight(int src, int dst) const
{
    auto& vws = adj.at(src);
    for (auto& [v, w] : vws) {
        if (v == dst)
            return w;
    }
}

void Graph::draw(const char* filename)
{
    // set up a graphviz context
    GVC_t* gvc = gvContext();

    // Create a simple digraph
    Agraph_t* g = agopen("g", Agdirected, 0);

    for (auto& uv : adj) {
        auto& u = uv.first;
        auto& neighbours = uv.second;
        for (auto& [v, w] : neighbours) {
            Agnode_t* n = agnode(g, const_cast<char*>(std::to_string(u).c_str()), 1);
            Agnode_t* m = agnode(g, const_cast<char*>(std::to_string(v).c_str()), 1);
            agsafeset(n, "shape", "box", "");
            agsafeset(n, "width", "0.1", "");
            agsafeset(n, "height", "0.1", "");
            agsafeset(m, "shape", "box", "");
            agsafeset(m, "width", "0.1", "");
            agsafeset(m, "height", "0.1", "");
            Agedge_t* e = agedge(g, n, m, 0, 1);
        }
    }
    agsafeset(g, "overlap", "false", "");
    // Set an attribute - in this case one that affects the visible rendering
    // agsafeset(n, "color", "red", "");

    // Compute a layout using layout engine from command line args
    gvLayout(gvc, g, "neato");
    gvRenderFilename(gvc, g, "png", (std::string(filename) + ".png").c_str());
    // Write the graph according to -T and -o options
    gvRenderJobs(gvc, g);

    // Free layout data
    gvFreeLayout(gvc, g);

    // Free graph structures
    agclose(g);

    // close output file, free context, and return number of errors
    gvFreeContext(gvc);
}
}
