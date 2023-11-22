#include "algorithm.h"
#include <queue>
#include <unordered_set>
#include <utility>
namespace simnet::algorithms {
using std::make_pair;
using std::pair;
using std::priority_queue;
using std::unordered_set;
using diPair = pair<double, int>;
void floyd_warshall(Mat<double>& distance)
{
    int n = distance.size();
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

}

}
