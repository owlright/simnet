package range.mod;

simple Controller
{
    parameters:
        string name=fullName();
        string trafficPattern = default("");
        //below are for manual set aggregation group which is helpful for debuging
        object aggrouter = default([]);
        object aggrnumber = default([]);
        object aggrbuffer = default([]);
        object targets = default([]);
        @display("i=block/cogwheel");
}