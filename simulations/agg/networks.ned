package simnet.simulations.agg;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;

network AggNto1
{
    parameters:
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
        }
        gate: Switch {
            @node;
            address = 1234;
        }
        sink: Host {
            @node;
            address = parent.numberOfSenders;
        }
        routeManager: GlobalRouteManager;
        groupManager: GlobalGroupManager;

    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}

