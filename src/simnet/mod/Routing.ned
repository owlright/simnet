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
        bool ecmpFlow = default(false);
        string aggPolicy = default("Count");
        int bufferSize @unit(B) = default(10MB);
        double collectPeriod @unit(s)= default(10us);

        @class(Routing);
        @display("i=block/switch");
        @signal[drop](type="long");
        @signal[outputIf](type="long");
        @signal[group*-usedBuffer](type=long);
        // @statistic[packets](title="outputPackets"; source=outputPacket; record=count; unit=pk);
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


