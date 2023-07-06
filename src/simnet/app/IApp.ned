//
// This file is part of an OMNeT++/OMNEST simulation example.
//
// Copyright (C) 1992-2015 Andras Varga
//
// This file is distributed WITHOUT ANY WARRANTY. See the file
// `license' for details on this and other legal matters.
//

package simnet.app;

//
// Interface for network traffic generator modules that
// can be used in Node.
//
moduleinterface IApp
{
    parameters:
        @display("i=block/app_s");
       int address;
       int port;

    gates:
        input in;
        output out;
}


