//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//


//
// Represents a packet in the network.
//
packet Packet
{
    int srcAddr @packetData;
    int destAddr @packetData;
    int groupAddr @packetData;
    int aggrCounter @packetData;
    int hopCount @packetData;
    unsigned int seq @packetData;
    unsigned int ackSeq @packetData;
    bool ECN @packetData;
}

