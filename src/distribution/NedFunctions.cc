#include "CdfDistribution.h"

int64_t cdf(cRNG *rng, std::string id) {
    return CdfDistribution::get(id).random(rng);
}

int64_t cdfMean(std::string id) {
    return CdfDistribution::get(id).mean();
}

cValue nedf_Cdf(cComponent *contextComponent, cValue argv[], int argc) {
    auto id = argv[0].stringValue();
    auto rng = argc == 2 ? argv[1].intValue() : 0;
    return cValue(cdf(contextComponent->getRNG(rng), id), "B");
}

Define_NED_Function2(nedf_Cdf,
                     "int cdf(string id, int rng?)",
                     "random/discrete",
                     "Returns a random number based on a cdf");

cValue nedf_cdfMean(cComponent *contextComponent, cValue argv[], int argc) {
    auto id = argv[0].stringValue();
    return cValue(cdfMean(id), "B");
}

Define_NED_Function2(nedf_cdfMean,
                     "int cdf_mean(string id)",
                     "random/discrete",
                     "Returns the mean of cdf");

