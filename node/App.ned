//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package node;

//
// Application model to generate traffic for the network.
//
module App like IApp
{
    parameters:
        int address;  // local node address
        int groupAddress = default(-1); // broadcast addr
        int packetTotalCount = default(-1); // total sending packets, 0 means initify
        int destAddress = default(-1); //
		double startTime @unit(s) = default(0s);
//        volatile double flowInterval @unit(s) = default(exponential(1s)); // time between generating flows
        int packetLength @unit(byte) = default(1KiB);  // length of one message (fixed! no "volatile" modifier)
        @class(App);
        @display("i=block/browser");
        @signal[endToEndDelay](type="simtime_t");
        @signal[hopCount](type="long");
        @signal[sourceAddress](type="long");
        @statistic[endToEndDelay](title="end-to-end delay of arrived packets";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[hopCount](title="hop count of arrived packets";interpolationmode=none;record=vector?,mean,max);
        @statistic[sourceAddress](title="source address of arrived packets";interpolationmode=none;record=vector?);

    gates:
        input in;
        output out;
        input socketIn @directIn;

    submodules:
        socket: Socket;

    connections:
        in --> socket.in;
		out <-- socket.out;
 }


