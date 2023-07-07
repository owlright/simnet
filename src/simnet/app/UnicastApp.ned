package simnet.app;
import simnet.mod.cong.Cong;
simple UnicastApp like IApp {
	parameters:
	    @class(UnicastApp);
        int address @mutable = default(-1);
        int port @mutable = default(65535); // ! I only change this when SRWorker is created, whey need mutable here? why destPort don't need mutable
    gates:
        input in;
        output out;
}

module UnicastSenderApp extends UnicastApp {
    parameters:
        @class(UnicastSenderApp);
        port = default(-1);
        int destAddress @mutable = default(-1);
        int destPort = default(1000);
        int messageLength @unit(B) = default(1kB);

        int flowSize @unit(B) = default(8kB); // in bytes
        double flowStartTime @unit(s) = default(0.0s);
//        volatile double flowInterval @unit(s) = default(0s); // this means load = 1.0
        bool useJitter = default(false); // ! set this to true before using jitter
        volatile double jitterBeforeSending @unit(s) = default(0s);
        @signal[fct](type=simtime_t);
        @signal[idealFct](type=simtime_t);
        @signal[flowSize](type=int);
        @signal[rtt](type=simtime_t);
        @signal[inflightBytes](type=int);
        @statistic[fct](title="flow complete time"; source=fct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[idealFct](title="ideal flow complete time"; source=idealFct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[flowSize](title="flow total bytes"; source=flowSize; unit=B; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[rtt](title="rtt"; source=rtt; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[inflightBytes](title="inflight bytes"; source=inflightBytes; unit=B; record=count,mean,max,min,vector,timeavg; interpolationmode=linear);
   submodules:
        cong: <default("Reno")> like Cong;
}

module UnicastEchoApp extends UnicastApp {
    parameters:
        port = default(1001);
        @class(UnicastEchoApp);
}

// module ATPWorker extends UnicastSenderApp {
//     parameters:
//         port = default(2000);
//         destPort = default(2001);
//         int groupAddress = default(this.destAddress);
//         @class(ATPWorker);
// }

module ParameterServerApp extends UnicastEchoApp {
    parameters:
        port = default(1000);
        int jobId = default(-1);
        int groupAddress = default(-1);
        int numWorkers = default(0);
        @class(ParameterServerApp);
        @signal[aggRatio](type=double);
        @statistic[aggRatio](title="aggegation ratio of a packet"; source=aggRatio; record=count,mean,max,min,vector; interpolationmode=linear);
}
