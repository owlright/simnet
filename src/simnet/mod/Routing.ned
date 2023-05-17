//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package simnet.mod;

//
// Models a router.
//
module Routing
{
    parameters:
        // ! these parameters are set in Node.ned
        // string routeManager = default("");
        bool ecmpFlow = default(false);
        string aggPolicy = default("Count");
        // string groupManager = default("");
        int bufferSize @unit(B) = default(10MB);

        @class(Routing);
        @display("i=block/switch");
        @signal[drop](type="long");
        @signal[outputIf](type="long");
        @signal[outputPacket](type=Packet);
        @signal[group*-usedBuffer](type=long);
        @statistic[packets](title="outputPackets"; source=outputPacket; record=count; unit=pk);
        @statistic[dataRate](title="data rate"; source=throughput(outputPacket)/1000; record=vector; unit=kbps; interpolationmode=linear);
        @statistic[drop](title="dropped packet byte length";unit=bytes;record=vector?,count,sum;interpolationmode=none);
        @statistic[outputIf](title="output interface for each routed packet";record=histogram;interpolationmode=none);
        @statisticTemplate[groupUsedBuffer](title="group used buffer on switch";record=mean,vector);

    gates:
        input in[];
        output out[];
        input localIn;
        output localOut;

    // submodules:
    //     pkh: <default("GroupPacketHandler")> like PacketHandler if this.groupManager != "";
}


