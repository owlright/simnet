package simnet.mod;

simple Controller
{
    parameters:
        string name=fullName();
        string trafficPattern = default("");
        string aggrGroupPlacePolicy = default("");
        bool shareAggrBuffer = true; // whether all aggr groups share the switch SRAM
        //below are for manual set aggregation group which is helpful for debuging
        object aggSenders = default([]);
        object aggRouters = default([]);
        object aggFanInDegree = default([]);
        object aggMaxBufferSize = default([]); // -1 means unlimited
        object targets = default([]);
        @display("i=block/cogwheel");
}