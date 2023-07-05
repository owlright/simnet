package simnet.mod.manager;

module GlobalGroupManager
{
    parameters:
        string groupSwitchFile = default("");
        string groupHostFile = default("");
        string placementPolicy = default("");
        int numGroups @mutable = default(1);
        int numWorkers @mutable = default(6);
        int numPSes @mutable = default(1);
        string aggTreeType = default("sptree"); // ATP, sptree
        @class(GlobalGroupManager);
        @display("i=block/cogwheel");
        @signal[group*-RoundFinishTime](type=simtime_t); // note the wildcard
        @statisticTemplate[groupRoundFinishTime](record=count,mean,max,min,vector; interpolationmode=none);
}