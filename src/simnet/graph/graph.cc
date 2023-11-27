#include "graph.h"
#include "algorithms.h"
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
        adjout[src].push_back(EdgeWeight(dest, weight));
        adjin[dest].push_back(EdgeWeight(src, weight));
        if (bidirectional) {
            adjout[dest].push_back(EdgeWeight(src, weight));
            adjin[src].push_back(EdgeWeight(dest, weight));
        }
    } else {
        set_weight(src, dest, weight);
    }
}

void Graph::set_weight(int src, int dest, double weight)
{
    ASSERT(has_edge(src, dest));
    ASSERT(adjout.find(src) != adjout.end());
    ASSERT(adjin.find(dest) != adjin.end());
    for (auto& x : adjout.at(src)) {
        if (x.first == dest) {
            x.second = weight;
            break;
        }
    }
    for (auto& x : adjin.at(dest)) {
        if (x.first == src) {
            x.second = weight;
            break;
        }
    }
}

void Graph::remove_edge(int src, int dest)
{
    ASSERT(adjin.find(dest) != adjin.end());
    ASSERT(adjout.find(src) != adjout.end());
    if (has_edge(src, dest)) {
        auto remove_adj = [](vector<EdgeWeight>& vw, const int& node) {
            int index = 0;
            for (auto& [v, w] : vw) {
                if (v == node) {
                    break;
                }
                index++;
            }
            vw.erase(vw.begin() + index);
        };
        remove_adj(adjout[src], dest);
        remove_adj(adjin[dest], src);
    }
}

void Graph::add_node(int n)
{
    if (!has_node(n)) {
        if (n > max_vertice)
            max_vertice = n;
        nodes.push_back(n);
        adjout[n] = vector<EdgeWeight>();
        adjin[n] = vector<EdgeWeight>();
    }
}

void Graph::remove_node(int n)
{
    // ! delete a node also delete all its edges
    if (has_node(n)) {
        adjout.erase(n);
        adjin.erase(n);
        auto it = std::find(nodes.begin(), nodes.end(), n);
        if (it != nodes.end())
            nodes.erase(it);
        if (n == max_vertice) {
            it = std::max_element(nodes.begin(), nodes.end());
            max_vertice = *it;
        }
    }
}
bool Graph::has_node(int n) const { return is_in_vector(n, nodes); }

bool Graph::has_edge(const int& src, const int& dest) const
{
    if (adjout.find(src) == adjout.end()) {
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

bool Graph::has_edge(const Edge& e) const
{
    auto& src = e.first;
    auto& dest = e.second;
    return has_edge(src, dest);
}

void Graph::update_dist()
{
    int n = get_max_vertice() + 1;
    dist = (double**)malloc(n * sizeof(double*));
    for (int i = 0; i < n; i++) {
        dist[i] = (double*)malloc(n * sizeof(double));
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            dist[i][j] = INFINITY;
        }
    }
    for (auto& uv : adjout) {
        auto& u = uv.first;
        auto& neighbours = uv.second;
        for (auto& [v, w] : neighbours) {
            dist[u][v] = w;
        }
    }
    floyd_warshall(dist, n);
}

double** Graph::get_dist() const
{
    // ASSERT(dist != nullptr); // ! let outside to check
    return dist;
}

double Graph::distance(int src, int dest) const
{
    if (dist) {
        return dist[src][dest];
    } else {
        return algorithms::dijistra(*this, src, dest);
    }
}

double Graph::weight(int src, int dst) const
{
    ASSERT(adjout.find(src) != adjout.end());
    auto& vws = adjout.at(src);
    for (auto& [v, w] : vws) {
        if (v == dst)
            return w;
    }
    throw cRuntimeError("edge %d->%d not found!", src, dst);
}

vector<int> Graph::dfs(int root, bool directionOut) const
{
    ASSERT(has_node(root));
    vector<int> result { root };
    stack<int> st;
    st.push(root);
    unordered_set<int> visited { root };
    auto& adj = directionOut ? adjout : adjin;
    while (!st.empty()) {
        auto u = st.top();
        st.pop();
        if (visited.find(u) == visited.end()) {
            visited.insert(u);
            result.push_back(u);
        }
        for (auto& [v, w] : adj.at(u)) {
            if (visited.find(v) == visited.end())
                st.push(v);
        }
    }
    return result;
}

vector<int> Graph::bfs(int root, bool directionOut) const
{
    ASSERT(has_node(root));
    vector<int> result { root };
    queue<int> que;
    que.push(root);
    unordered_set<int> visited { root };
    auto& adj = directionOut ? adjout : adjin;
    while (!que.empty()) {
        auto u = que.front();
        que.pop();
        if (visited.find(u) == visited.end()) {
            visited.insert(u);
            result.push_back(u);
        }
        for (auto& [v, w] : adj.at(u)) {
            if (visited.find(v) == visited.end())
                que.push(v);
        }
    }
    return result;
}


void Graph::draw(const char* filename, const char* engine)
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

    for (auto& uv : adjout) {
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
    // agsafeset(g, "splines", "true", "");

    // Compute a layout using layout engine from command line args
    gvLayout(gvc, g, engine);
    const char* dotPos = strrchr(filename, '.');
    if (dotPos) {
        const char * ext = dotPos + 1;
        gvRenderFilename(gvc, g, ext, filename);
    }
    else {
        gvRenderFilename(gvc, g, "png", (std::string(filename) + ".png").c_str());
    }

    // Write the graph according to -T and -o options
    gvRenderJobs(gvc, g);

    // Free layout data
    gvFreeLayout(gvc, g);

    // Free graph structures
    agclose(g);

    // close output file, free context, and return number of errors
    gvFreeContext(gvc);
}
Graph::Graph() { }
Graph::~Graph()
{
    if (dist) {
        for (int i = 0; i < max_vertice + 1; i++) {
            free(dist[i]);
        }
        free(dist);
    }
}
Graph::Graph(const Graph& other)
{
    nodes = other.nodes;
    max_vertice = other.max_vertice;
    adjin = other.adjin;
    adjout = other.adjout;

    if (other.get_dist()) {
        int n = other.get_max_vertice() + 1;
        dist = (double**)malloc(n * sizeof(double*));
        for (int i = 0; i < n; i++) {
            dist[i] = (double*)malloc(n * sizeof(double));
            memcpy(dist[i], other.dist[i], n * sizeof(double));
        }
    }
}
Graph& Graph::operator=(const Graph& other)
{
    if (this != &other) {
        nodes = other.nodes;
        max_vertice = other.max_vertice;
        adjin = other.adjin;
        adjout = other.adjout;
        dist = other.dist;
    }
    return *this;
}

Graph::Graph(Graph&& other)
{
    // Transfer ownership of the memory
    dist = other.dist;
    other.dist = nullptr;
    nodes = std::move(other.nodes);
    max_vertice = other.max_vertice;
    adjin = std::move(other.adjin);
    adjout = std::move(other.adjout);
}

Graph& Graph::operator=(Graph&& other)
{
    if (this != &other) {
        // Transfer ownership of the memory
        dist = other.dist;
        other.dist = nullptr;
        nodes = std::move(other.nodes);
        max_vertice = other.max_vertice;
        adjin = std::move(other.adjin);
        adjout = std::move(other.adjout);
    }
    return *this;
}

}
