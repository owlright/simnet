#include "utils.h"
#include <algorithm>
#include <cstdlib>
#include <random>
#include <unordered_set>
bool sendToItself(const std::vector<int>& p)
{
    for (auto i = 0; i < p.size(); i++) {
        if (i == p[i]) {
            return true;
        }
    }
    return false;
}
void permutation(std::vector<std::vector<int>>& mat, uint32_t seed)
{
    auto n = mat.size();
    std::vector<int> permutation(n);
    for (auto i = 0; i < n; i++)
        permutation[i] = i;
    uint32_t _tmp = 0;
    while (sendToItself(permutation)) {
        std::shuffle(permutation.begin(), permutation.end(), std::default_random_engine(seed+ _tmp++));
    }

    for (auto i = 0; i < n; i++) {
        auto one_index = permutation[i];
        for (auto j = 0; j < n; j++) {
            mat[i][j] = j == one_index ? 1 : 0;
        }
    }
}
