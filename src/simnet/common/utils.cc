#include "utils.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <unordered_set>
void permutation(std::vector<std::vector<int>>& mat, uint32_t seed)
{
    auto n = mat.size();
    std::vector<int> permutation(n);
    for (auto i = 0; i < n; i++)
        permutation[i] = i;
    std::unordered_set<int> not_used(permutation.begin(), permutation.end());
    std::srand(seed);
    for (int i = 0; i < n; i++) {
        int j = -1;
        do {
            auto it = std::begin(not_used);
            std::advance(it, std::rand() % not_used.size());
            j = *it;
        } while (i == j); // avoid send to oneself
        permutation[i] = j;
        not_used.erase(j);
    }

    for (auto i = 0; i < n; i++) {
        auto one_index = permutation[i];
        for (auto j = 0; j < n; j++) {
            mat[i][j] = j == one_index ? 1 : 0;
        }
    }
}
