package simnet.mod.manager;

simple TrafficPatternManager {
    parameters:
        string trafficPattern = default("");
        @display("i=block/cogwheel");
        @class(TrafficPatternManager);
}