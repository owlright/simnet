package simnet.mod.manager;

module GlobalMetricCollector
{
    parameters:
        @class(GlobalMetricCollector);
        @display("i=block/cogwheel");
        @signal[job-*-RoundFinishTime](type=simtime_t); // note the wildcard
        @signal[jobRCT](type=simtime_t);
        @statisticTemplate[jobRoundCompleteTime](record=vector; interpolationmode=none);
        @statistic[jobRCT](record=vector; source=jobRCT; interpolationmode=none);
}