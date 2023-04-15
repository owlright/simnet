package mnet.networks;

import mnet.mod.Host;
import mnet.mod.Switch;
import ned.DatarateChannel;
import mnet.mod.Controller;

network AggrDumbell
{
    parameters:
        string globalController = fullName()+".controller";

    types:
        channel C extends DatarateChannel
        {
            delay = 10us;
            datarate = 10Gbps;
        }

    submodules:
        controller: Controller;
        group1[1]: Host {
            address = index;
            globalController = parent.globalController;
        }
        aggr1: Switch {
            address = 100;
            globalController = parent.globalController;
        }
        group2[16]: Host {
            address = index+8;
            globalController = parent.globalController;
        }
        aggr2: Switch {
            address = 200;
            globalController = parent.globalController;
        }
        gate: Switch {
            address = 300;
            globalController = parent.globalController;
        }
        gateSink: Switch {
            address = 301;
            globalController = parent.globalController;
        }
        sink1: Host {
            address = 40;
            globalController = parent.globalController;
        }
        sink2: Host {
            address = 41;
            globalController = parent.globalController;
        }
    connections:
//        for i=0..7 {
            group1[0].port++  <--> C <--> aggr1.port++;
//        }
        for i=0..15 {
            group2[i].port++  <--> C <--> aggr2.port++;
        }
        aggr1.port++  <--> C <-->  gate.port++;
        aggr2.port++  <--> C <-->  gate.port++;
        gate.port++ <--> C <--> gateSink.port++;
        gateSink.port++ <--> C <--> sink1.port++;
        gateSink.port++ <--> C <--> sink2.port++;
}


network AggrNto1
{
     parameters:
        string globalController = fullName()+".controller";
        int numberOfSenders = default(2);

    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }
    submodules:
        sender[numberOfSenders]: Host {
            @node;
            address = index;
            globalController = parent.globalController;
        }
        gate: Switch {
            @node;
            address =666;
            globalController = parent.globalController;
        }
        sink: Host {
            @node;
            address = parent.numberOfSenders;
            globalController = parent.globalController;
        }
        controller: Controller;
    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}