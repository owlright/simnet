#pragma once

#include <vector>

#include "inet/common/INETDefs.h"

namespace inet {

struct CdfPoint {
    int64_t length; // unit: Byte
    double p;
};

using Cdf = std::vector<CdfPoint>;

class CdfDistribution {
  private:
    const Cdf cdf;

  private:
    static std::map<std::string, CdfDistribution> table;
    static const CdfDistribution &add(const std::string file);
    static const CdfDistribution &add(const std::string id, const Cdf cdf);

  public:
    static const CdfDistribution &get(std::string id);

    CdfDistribution(Cdf cdf);
    int64_t random(cRNG *rng) const;
    int64_t mean() const;
};

} // namespace inet
