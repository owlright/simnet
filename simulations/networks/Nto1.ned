package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
network Nto1
{
    parameters:
        string globalRouteManager = fullName() + ".routeManager";
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
            address = 1234;
//            globalRouteManager = parent.globalRouteManager;
        }
        sink: Host {
            @node;
            address = parent.numberOfSenders;
//            globalRouteManager = parent.globalRouteManager;
        }
        routeManager: GlobalRouteManager;
    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}

