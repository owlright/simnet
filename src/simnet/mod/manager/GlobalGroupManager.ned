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
        double costThreshold = default(-1); // No equal agg nodes
        int numECTrees = default(-1); // use as many as can be found
        @class(GlobalGroupManager);
        @display("i=block/cogwheel");
}