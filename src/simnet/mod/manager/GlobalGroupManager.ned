package simnet.mod.manager;

simple GlobalGroupManager
{
    parameters:
        string groupSwitchFile;
        string groupHostFile;
        string groupPolicy = "random";
        @class(GlobalGroupManager);
        @display("i=block/cogwheel");
        @signal[group*-RoundFinishTime](type=simtime_t); // note the wildcard
        @statisticTemplate[groupRoundFinishTime](record=count,mean,max,min,vector; interpolationmode=none);
}