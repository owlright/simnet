package simnet.mod.cong;

simple Dctcp extends Reno
{
    parameters:
        double alpha = default(1.0);
        double g = default(0.0625);
        @signal[alpha](type="double");
        @statistic[alpha](title="estimated congestion";record=vector;interpolationmode=linear);
        @signal[eceRatio](type="double");
        @statistic[eceRatio](title="packets labeled ECN in last RTT";record=vector;interpolationmode=linear);
        @class(Dctcp);
}