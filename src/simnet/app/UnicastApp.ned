package simnet.app;
import simnet.mod.cong.Cong;
simple UnicastApp like IApp {
	parameters:
	    @class(UnicastApp);
        int address = default(-1);  // local node address
        int port = default(0);
    gates:
        input in;
        output out;
}

module UnicastSenderApp extends UnicastApp {
   parameters:
       @class(UnicastSenderApp);
       port = default(1000);
        int destAddress = default(-1);
        string destAddresses = default("");
        int destPort = default(1001);
        int messageLength @unit(B) = default(1kB);
        int numRounds = default(1);

        double load = default(0.0);
        int flowSizeMean @unit(B) = default(0B);
        // ! in case you want to set flowSize and flowInterval manually
        volatile int flowSize @unit(B) = default(8kB); // in bytes
        volatile double flowInterval @unit(s) = default(0s); // this means load = 1.0
        volatile double jitterBeforeSending @unit(s) = default(0s);
        @signal[fct](type=simtime_t);
        @signal[idealFct](type=simtime_t);
        @signal[flowSize](type=int);
        @signal[rtt](type=simtime_t);
        @statistic[fct](title="flow complete time"; source=fct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[idealFct](title="ideal flow complete time"; source=idealFct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[flowSize](title="ideal flow complete time"; source=flowSize; unit=B; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[rtt](title="rtt"; source=rtt; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);

   submodules:
        cong: <default("Reno")> like Cong;
}

module UnicastEchoApp extends UnicastApp {
    parameters:
        port = default(1001);
        @class(UnicastEchoApp);
}

module WorkerApp extends UnicastSenderApp {
    parameters:
        port = default(2000);
        destPort = default(2001);
        int groupAddress = default(this.destAddress);
        @class(WorkerApp);
}

module ParameterServerApp extends UnicastEchoApp {
    parameters:
        port = default(2001);
        @class(ParameterServerApp);
        @signal[aggRatio](type=double);
        @statistic[aggRatio](title="aggegation ratio of a packet"; source=aggRatio; record=count,mean,max,min,vector; interpolationmode=linear);
}

module TimerWorkerApp extends WorkerApp {
    parameters:
        @class(TimerWorkerApp);
}