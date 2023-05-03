package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Controller;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Dumbbell
{
    parameters:
        string globalRouteManager = fullName()+".controller";
        int numberFlows = default(1);
    types:
        channel C extends DatarateChannel
        {
            delay = 1us;
            datarate = 10Gbps;
        }

    submodules:
        sink[numberFlows]: Host {
            @node;
//            @display("p=,,c,40");
            address = index;
//            globalRouteManager = parent.globalRouteManager;
        }
        sender[numberFlows]: Host {
            @node;
//            @display("p=,,c,40");
            address = 10*parent.numberFlows + index;
            app.destAddress = index;
//            globalRouteManager = parent.globalRouteManager;
        }
        LR: Host {
            @node;
//            @display("g=bottleneck;p=,,r,100");
            address = 1234;
//            globalRouteManager = parent.globalRouteManager;
        }
        RR: Host {
            @node;
//            @display("g=bottleneck;p=,,r,100");
            address = 4321;
//            globalRouteManager = parent.globalRouteManager;
        }
        controller: Controller;
    connections:
        for i=0..numberFlows-1 {
            sender[i].port++ <--> C <--> LR.port++;
        }
        for i=0..numberFlows-1 {
            sink[i].port++ <--> C <--> RR.port++;
        }
        LR.port++ <--> C <--> RR.port++;

}
