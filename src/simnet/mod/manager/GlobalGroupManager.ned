package simnet.mod.manager;

simple GlobalGroupManager
{
    parameters:
        string groupSwitchFile;
        string groupHostFile;
        @class(GlobalGroupManager);
        @display("i=block/cogwheel");
}