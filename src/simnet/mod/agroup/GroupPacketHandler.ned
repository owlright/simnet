package simnet.mod.agroup;
import simnet.mod.PacketHandler;

simple GroupPacketHandler like PacketHandler
{
    parameters:
        @class(GroupPacketHandler);
        string groupManager;
        int bufferSize @unit(B) = default(10MB);
}