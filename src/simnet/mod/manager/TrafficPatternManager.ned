package simnet.mod.manager;

module TrafficPatternManager {
    parameters:
        string trafficPattern = default("");
        @display("i=block/cogwheel");
        @class(TrafficPatternManager);
}