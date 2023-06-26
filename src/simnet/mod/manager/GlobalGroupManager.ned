package simnet.mod.manager;

simple GlobalGroupManager
{
    parameters:
        string groupSwitchFile = default("");
        string groupHostFile = default("");
        string placementPolicy = default("random");
        int numGroups @mutable = 5;
        int numWorkers @mutable = 6;
        int numPSes @mutable = 1;
        string aggTreeType = default("sptree"); // ATP, sptree
        @class(GlobalGroupManager);
        @display("i=block/cogwheel");
        @signal[group*-RoundFinishTime](type=simtime_t); // note the wildcard
        @statisticTemplate[groupRoundFinishTime](record=count,mean,max,min,vector; interpolationmode=none);
}