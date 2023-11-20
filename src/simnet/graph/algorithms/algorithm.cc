#include "algorithm.h"
namespace simnet::algorithms {
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
}