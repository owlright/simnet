#include "FbHdp_distribution.inc"
#include "AliStorage2019.inc"
#include "WebSearch_distribution.inc"
#include "GoogleRPC2008.inc"

std::map<std::string, CdfDistribution> CdfDistribution::table{
    {"FbHdp_distribution", FbHdp_distribution},
    {"AliStorage2019", AliStorage2019},
    {"WebSearch_distribution", WebSearch_distribution},
    {"GoogleRPC2008", GoogleRPC2008},
};
