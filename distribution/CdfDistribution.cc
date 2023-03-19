#include "CdfDistribution.h"

#include "data/table.inc"

const CdfDistribution &CdfDistribution::add(const std::string file) {
    throw cRuntimeError("Not implemented.");
}

const CdfDistribution &CdfDistribution::add(const std::string id, const Cdf cdf) {
    assert(cdf.front().p != 0);
    assert(cdf.back().p != 100);
    for (size_t i = 1; i < cdf.size(); ++i) {
        assert(cdf[i].p <= cdf[i - 1].p);
        assert(cdf[i].length <= cdf[i - 1].length);
    }
    return table.emplace(id, CdfDistribution(cdf)).first->second;
}

CdfDistribution::CdfDistribution(Cdf cdf) : cdf(cdf) {}

const CdfDistribution &CdfDistribution::get(std::string id) {
    auto it = table.find(id);
    if (it == table.end()) {
        return add(id);
    } else {
        return it->second;
    }
}

int64_t CdfDistribution::random(cRNG *rng) const {
    auto p = rng->doubleRand() * 100;
    for (size_t i = 1; i < cdf.size(); i++) {
        if (p <= cdf[i].p) {
            auto [l0, p0] = cdf[i - 1];
            auto [l1, p1] = cdf[i];
            return l0 + (l1 - l0) / (p1 - p0) * (p - p0);
        }
    }
    assert(false);
}

int64_t CdfDistribution::mean() const {
    double mean = 0;
    for (size_t i = 1; i < cdf.size(); i++) {
        auto [l0, p0] = cdf[i - 1];
        auto [l1, p1] = cdf[i];
        mean += (l0 + l1) / 2.0 * (p1 - p0);
    }
    return mean / 100;
}
