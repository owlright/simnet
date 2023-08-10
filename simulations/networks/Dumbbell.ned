package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.manager.GlobalRouteManager;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Dumbbell
{
    parameters:
        string globalRouteManager = fullName()+".controller";
        int numberFlows = default(1);
        @display("bgb=990,348");
    types:
        channel C extends DatarateChannel
        {
            delay = 1us;
            datarate = 10Gbps;
        }

    submodules:
        sink[numberFlows]: Host {
            address = index;
            @display("p=605,150,c,50");
        }
        sender[numberFlows]: Host {
            destAddress = index;
            @display("p=164,150,c,50");
        }
        LR: Host {
            @display("p=300,150");
        }
        RR: Host {
            @display("p=440,150");
        }
        routeManager: GlobalRouteManager {
            @display("p=878,50");
        }

    connections:
        for i=0..numberFlows-1 {
            sender[i].port++ <--> C <--> LR.port++;
        }
        for i=0..numberFlows-1 {
            sink[i].port++ <--> C <--> RR.port++;
        }
        LR.port++ <--> C <--> RR.port++;

}

