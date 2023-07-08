package simnet.mod.manager;

module GlobalMetricCollector
{
    parameters:
        @class(GlobalMetricCollector);
        @display("i=block/cogwheel");
        @signal[job-*-RoundFinishTime](type=simtime_t); // note the wildcard
        @statisticTemplate[jobRoundCompleteTime](record=vector; interpolationmode=none);
}