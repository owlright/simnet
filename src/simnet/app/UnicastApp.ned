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
        int destPort = default(1001);
        int messageLength @unit(B) = default(1kB);
        int flowSize @unit(B) = default(8kB);
        volatile double flowInterval @unit(s) = default(10us);
        @signal[fct](type=simtime_t);
        @singal[idealFct](type=simtime_t);
        @statistic[fct](title="flow complete time"; source=fct; record=count,mean,max,min,vector; interpolationmode=none);
        @statistic[idealFct](title="ideal flow complete time"; source=idealFct; record=count,mean,max,min,vector; interpolationmode=none);
   submodules:
        cong: <default("Reno")> like Cong;
}

module UnicastEchoApp extends UnicastApp {
    parameters:
        port = default(1001);
        @class(UnicastEchoApp);
//    submodules:
//        cong: <default("Reno")> like Cong;
}

module WorkerApp extends UnicastSenderApp {
    parameters:
        port = default(2000);
        destPort = default(2001);
        int groupAddress = default(this.destAddress);
        string groupManager;
        @class(WorkerApp);

}

module ParameterServerApp extends UnicastEchoApp {
    parameters:
        port = default(2001);
        string groupManager;
        @class(ParameterServerApp);
    
}