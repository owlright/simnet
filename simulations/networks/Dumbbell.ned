package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Switch;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalView;
import simnet.mod.manager.TrafficPatternManager;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Dumbbell
{
    parameters:
        int numberFlows = default(1);
        @display("bgb=989.02997,487.06");

    types:
        channel C extends DatarateChannel
        {
            delay = 1us;
            datarate = 10Gbps;
        }

    submodules:
        sink[numberFlows]: Host {
            address = index;
            @display("p=605,50,c,60");
        }
        sender[numberFlows]: Host {
            @display("p=164,50,c,60");
        }
        LR: Switch {
            @display("p=300,200");
        }
        RR: Switch {
            @display("p=440,200");
        }
        routeManager: GlobalRouteManager {
            @display("p=883.4175,149.09999");
        }
        globalView: GlobalView {
            @display("p=883.4175,47.215");
        }
        tpManager: TrafficPatternManager {
            @display("p=428.01373,237.915");
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

