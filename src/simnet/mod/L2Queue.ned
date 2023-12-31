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
// Represents the network interface for a point-to-point connection.
//
// Packets arriving from the upper layer ("in" gate) are sent out on
// the "line" gate, which is expected to be connected to a link with
// nonzero data rate. Packets that arrive while a previous packet is
// being transmitted are queued up. The maximum queue size in packets
// can be specified in a parameter. Excess frames are simply discarded
// and recorded as statistics.
//
// The model can be easily extended in several ways: to make it possible to
// query the queue length from another module via a direct method call
// interface, to implement priority queueing, or to collect link statistics
// (utilization, etc.)
//
simple L2Queue
{
    parameters:
        int capacity @unit(B) = default(10MB); // max bytes a queue can hold; -1 means no limit;
        int ecnThreshold = default(10); // ! threshold when begining to mark the packet; unit is frameSize
        int frameSize @unit(B) = default(1kB);
        bool useCutThroughSwitching = default(false);  // use cut-through switching instead of store-and-forward
        @display("i=block/queue;q=queue");
        @signal[qlen](type="long");
        // @signal[busy](type="bool");
        // @signal[congestion](type="long");
        // @signal[queueingTime](type="simtime_t");
        // @signal[drop](type="long");
        // @signal[txBytes](type="long");
        // @signal[rxBytes](type="long");
        @signal[outputPacket](type=Packet);

        @statistic[qlen](title="queue length(Byte)";record=vector,max;interpolationmode=linear);
        // @statistic[busy](title="server busy state";record=vector?,timeavg;interpolationmode=sample-hold);
        // @statistic[congestion](title="congestion happend";record=vector;interpolationmode=sample-hold);
        // @statistic[queueingTime](title="queueing time at dequeue";unit=s;interpolationmode=none);
        // @statistic[drop](title="dropped packet byte length";unit=bytes;record=vector?,count,sum;interpolationmode=none);
        // @statistic[txBytes](title="transmitting packet byte length";unit=bytes;record=vector?,count,sum,histogram;interpolationmode=none);
        // @statistic[rxBytes](title="received packet byte length";unit=bytes;record=vector?,count,sum,histogram;interpolationmode=none);
        @statistic[dataRate](title="data rate"; source=throughput(outputPacket)/1e6; record=vector; unit=Mbps; interpolationmode=linear);
    gates:
        input in;
        output out;
        inout line;
}


