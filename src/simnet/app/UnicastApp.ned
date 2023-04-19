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
        int destAddress = default(-1);
        int destPort = default(0);
        int messageLength @unit(B) = default(1000B);
        int flowSize @unit(B) = default(10MB);
        volatile double flowInterval @unit(s) = default(10us);
   submodules:
        cong: <default("Reno")> like Cong;
}

module UnicastEchoApp extends UnicastApp {
    parameters:
        @class(UnicastEchoApp);
    submodules:
        cong: <default("Reno")> like Cong;
}