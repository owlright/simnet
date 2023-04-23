package simnet.mod.cong;

simple Reno like Cong
{
    parameters:
        int initWinSize @unit(B) = default(1000B);
        @class(Reno);
		@signal[cwnd](type=int);
        @statistic[cwnd](source=cwnd;title="Congestion Window";record=vector;interpolationmode=linear;);
}