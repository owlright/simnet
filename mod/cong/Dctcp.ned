package mnet.mod.cong;

simple Dctcp like Cong
{
    parameters:
        double alpha = default(1.0);
        double g = default(0.0625);
        bool aggressive = default(true);
        @signal[alpha](type="double");
        @statistic[alpha](title="queue length";record=vector;interpolationmode=none);
        @signal[packetsECN](type="double");
        @statistic[packetsECN](title="packets labeled ECN in last RTT";record=vector;interpolationmode=none);
        @class(TcpDctcp);
}