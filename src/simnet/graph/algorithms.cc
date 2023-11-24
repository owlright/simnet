#include "algorithms.h"
#include <queue>
#include <unordered_set>
#include <utility>
namespace simnet::algorithms {
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::unordered_set;
using diPair = pair<double, int>;
using CostPath = pair<double, Path>;

bool operator==(const std::vector<int>& vec1, const std::vector<int>& vec2)
{
    if (vec1.size() != vec2.size()) {
        return false; // Vectors have different sizes, they are not equal
    }

    for (size_t i = 0; i < vec1.size(); i++) {
        if (vec1[i] != vec2[i]) {
            return false; // Found a mismatching element, vectors are not equal
        }
    }

    return true; // All elements are equal, vectors are equal
}

void floyd_warshall(Mat<double>& distance, int n)
{
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                // Update the distance using vertex k as an intermediate point
                if (distance[i][k] != INFINITY && distance[k][j] != INFINITY) {
                    distance[i][j] = std::min(distance[i][j], distance[i][k] + distance[k][j]);
                }
            }
        }
    }
}
void floyd_warshall(double** distance, int n)
{
    for (int k = 0; k < n; ++k) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                // Update the distance using vertex k as an intermediate point
                if (distance[i][k] != INFINITY && distance[k][j] != INFINITY) {
                    distance[i][j] = distance[i][j] < distance[i][k] + distance[k][j] ? distance[i][j]
                                                                                      : distance[i][k] + distance[k][j];
                }
            }
        }
    }
}
// Comparison function to compare std::pair<double, int> based on the first element (double value)
struct CompareDiPair {
    bool operator()(const diPair& a, const diPair& b)
    {
        return a.first > b.first; // Comparing based on the first element (double value)
    }
};

double dijistra(const Graph& g, int src, int dest, vector<int>* path)
{
    int n = g.get_max_vertice() + 1;
    priority_queue<diPair, std::deque<diPair>, CompareDiPair> pq;
    vector<double> dist(n, INFINITY);
    vector<int> prev(n, -1);
    pq.push(make_pair(0.0, src));
    dist[src] = 0;
    if (src == dest) {
        if (path)
            path->push_back(src);
        return 0;
    }
    bool found = false;
    unordered_set<int> visited { src };
    while (!pq.empty()) {
        auto u = pq.top().second;
        pq.pop();

        for (auto& [v, weight] : g.out_neighbors(u)) {
            // If there is shorted path to v through u.
            if (visited.find(v) == visited.end() && dist[v] > dist[u] + weight) {
                // Updating distance of v
                visited.insert(v);
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.push(make_pair(dist[v], v));
            }
            if (v == dest) {
                found = true;
                break;
            }
        }
        if (found) {
            break;
        }
    }

    if (found) {
        if (path) {
            auto u = dest;
            while (u != -1) {
                path->insert(path->begin(), u);
                u = prev[u];
            }
        }
        return dist[dest];
    }
    return INFINITY; // * unreachable
}

vector<double> yenksp(Graph& g, int src, int dest, int K, vector<Path>& A)
{
    vector<double> distances;
    Path p;
    vector<CostPath> B;
    distances.push_back(dijistra(g, src, dest, &p));
    A.push_back(p);
    for (int k = 1; k < K; k++) {
        auto& prevPath = A.at(k - 1);
        for (int i = 0; i < prevPath.size() - 1; i++) {
            int spurNode = prevPath[i];
            vector<int> rootPath(prevPath.begin(), prevPath.begin() + i + 1);
            auto rootPathCost = dijistra(g, rootPath.front(), rootPath.back());
            vector<double> weights_to_delete;
            vector<Graph::Edge> edges_to_delete;
            for (auto& p : A) {
                vector<int> pslice(p.begin(), p.begin() + i + 1);
                if (rootPath == pslice) {
                    auto u = p.at(i);
                    auto v = p.at(i + 1);
                    if (g.has_edge(u, v)) {
                        edges_to_delete.push_back({ u, v });
                        weights_to_delete.push_back(g.weight(u, v));
                        g.remove_edge(u, v);
                    }
                }
            }
            // * remove rootPathNode from Graph except spurNode
            for (auto& rootPathNode : rootPath) {
                if (rootPathNode == spurNode) {
                    continue;
                }
                for (auto& [v, w] : g.out_neighbors(rootPathNode)) {
                    edges_to_delete.push_back({ rootPathNode, v });
                    weights_to_delete.push_back(w);
                }
                for (auto& [v, w] : g.in_neighbors(rootPathNode)) {
                    edges_to_delete.push_back({ v, rootPathNode });
                    weights_to_delete.push_back(w);
                }
                g.remove_node(rootPathNode);
            }

            vector<int> spurPath;
            auto spurPathCost = dijistra(g, spurNode, dest, &spurPath);
            if (spurPathCost != INFINITY) { // ! there may be no path from spurNode to dest
                spurPathCost += rootPathCost;
                ASSERT(spurPath.size() >= 2);
                vector<int> totalPath(rootPath.begin(), rootPath.end());
                totalPath.insert(totalPath.end(), spurPath.begin() + 1, spurPath.end());
                bool found = false;
                for (auto& [c, x] : B) {
                    if (x == totalPath) {
                        found = true;
                        break;
                    }
                }
                if (!found) {
                    B.push_back({ spurPathCost, totalPath });
                }
            } else {
                // do nothing
                ASSERT(spurPath.empty());
            }
            for (int index = 0; index < weights_to_delete.size(); index++) {
                g.add_edge(edges_to_delete[index].first, edges_to_delete[index].second, weights_to_delete[index]);
            }
        }
        if (B.empty())
            break;
        std::sort(B.begin(), B.end(), [](const auto& a, const auto& b) { return a.first < b.first; });
        distances.push_back(B[0].first);
        A.push_back(B[0].second);
        B.erase(B.begin());
    }
    return distances;
}

}
