package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.Controller;

network AggrDumbell
{
    parameters:
        string globalRouteManager = fullName()+".controller";

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
//            globalRouteManager = parent.globalRouteManager;
        }
        aggr1: Switch {
            address = 100;
//            globalRouteManager = parent.globalRouteManager;
        }
        group2[16]: Host {
            address = index+8;
//            globalRouteManager = parent.globalRouteManager;
        }
        aggr2: Switch {
            address = 200;
//            globalRouteManager = parent.globalRouteManager;
        }
        gate: Switch {
            address = 300;
//            globalRouteManager = parent.globalRouteManager;
        }
        gateSink: Switch {
            address = 301;
//            globalRouteManager = parent.globalRouteManager;
        }
        sink1: Host {
            address = 40;
//            globalRouteManager = parent.globalRouteManager;
        }
        sink2: Host {
            address = 41;
//            globalRouteManager = parent.globalRouteManager;
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
        string globalRouteManager = fullName()+".controller";
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
//            globalRouteManager = parent.globalRouteManager;
        }
        gate: Switch {
            @node;
            address =666;
//            globalRouteManager = parent.globalRouteManager;
        }
        sink: Host {
            @node;
            address = parent.numberOfSenders;
//            globalRouteManager = parent.globalRouteManager;
        }
        controller: Controller;
    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}