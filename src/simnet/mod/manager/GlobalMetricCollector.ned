package simnet.mod.manager;

module GlobalMetricCollector
{
    parameters:
        bool showProgressInfo = default(false);
        double progressInterval = default(10); // in seconds
        @class(GlobalMetricCollector);
        @display("i=block/cogwheel");
        @signal[job-*-RoundFinishTime](type=simtime_t); // note the wildcard
        @signal[jobRCT](type=simtime_t);
        @statisticTemplate[jobRoundCompleteTime](record=vector; interpolationmode=none);
        @statistic[jobRCT](record=vector; source=jobRCT; interpolationmode=none);
}