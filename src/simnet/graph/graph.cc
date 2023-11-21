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
    add_node(src);
    add_node(dest);
    if (!has_edge(src, dest)) {
        adj[src].push_back(EdgeWeight(dest, weight));
        adjreverse[dest].push_back(EdgeWeight(src, weight));
        if (bidirectional) {
            adj[dest].push_back(EdgeWeight(src, weight));
            adjreverse[src].push_back(EdgeWeight(dest, weight));
        }
    } else {
        int pos = 0;
        for (auto& x : adj.at(src)) {
            if (x.first == dest) {
                x.second = weight;
                break;
            }
            pos += 1;
        }
        adjreverse.at(dest).at(pos).second = weight;
    }
}

void Graph::add_node(int n)
{
    if (!has_node(n)) {
        nodes.push_back(n);
        adj[n] = vector<EdgeWeight>();
        adjreverse[n] = vector<EdgeWeight>();
    }
}

bool Graph::has_node(int n) { return is_in_vector(n, nodes); }

bool Graph::has_edge(const int& src, const int& dest)
{
    if (adj.find(src) == adj.end()) {
        return false;
    } else {
        const auto& outneighbours = out_neighbors(src);
        for (auto& x : outneighbours) {
            if (x.first == dest) {
                return true;
            }
        }
    }
    return false;
}

bool Graph::has_edge(const Edge& e)
{
    auto& src = e.first;
    auto& dest = e.second;
    return has_edge(src, dest);
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
    ASSERT(!dist.empty());
    return dist;
}

double Graph::distance(int src, int dest) const
{
    if (!dist.empty()) {
        return dist[src][dest];
    } else {
        return algorithms::dijistra(*this, src, dest);
    }
}

double Graph::weight(int src, int dst) const
{
    ASSERT(adj.find(src) != adj.end());
    auto& vws = adj.at(src);
    for (auto& [v, w] : vws) {
        if (v == dst)
            return w;
    }
    throw cRuntimeError("edge %d->%d not found!", src, dst);
}

void Graph::draw(const char* filename)
{
    // set up a graphviz context
    GVC_t* gvc = gvContext();

    // Create a simple digraph
    char gname[] = "network";
    char shape[] = "plaintext";
    char empty[] = "";
    char width[] = "0.1";
    char height[] = "0.1";
    char margin[] = "0.01";
    char overlap[] = "overlap";
    char False[] = "false";
    Agraph_t* g = agopen(gname, Agdirected, 0);

    for (auto& uv : adj) {
        auto& u = uv.first;
        auto& neighbours = uv.second;
        for (auto& [v, w] : neighbours) {
            auto n = agnode(g, const_cast<char*>(std::to_string(u).c_str()), 1);
            auto m = agnode(g, const_cast<char*>(std::to_string(v).c_str()), 1);
            agsafeset(n, "shape", shape, empty);
            agsafeset(n, "width", width, empty);
            agsafeset(n, "height", height, empty);
            agsafeset(n, "margin", margin, empty);
            agsafeset(m, "shape", shape, empty);
            agsafeset(m, "width", width, empty);
            agsafeset(m, "height", height, empty);
            agsafeset(m, "margin", margin, empty);
            auto e = agedge(g, n, m, 0, 1);
            agsafeset(e, "arrowsize", ".5", empty);
        }
    }
    agsafeset(g, overlap, False, empty);

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
