package simnet.mod.manager;

module TrafficPatternManager {
    parameters:
        string trafficPattern = default("");
        int idleHostsNumber = default(0); // < 0 means infinity
        @display("i=block/cogwheel");
        @class(TrafficPatternManager);
}