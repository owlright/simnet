package simnet.simulations.unicast;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;

channel C extends DatarateChannel
{
    delay = 2us;
    datarate = 10Gbps;
}

network Nto1
{
    parameters:
        int numberOfSenders = default(2);

    submodules:
        sender[numberOfSenders]: Host {
            address = index;
        }
        gate: Switch {
            address = 1234;
        }
        sink: Host {
            address = parent.numberOfSenders;
        }
        routeManager: GlobalRouteManager;

    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}

network straightline
{
    parameters:
        int numSwitches = default(1);
    submodules:
        sender: Host {
        	address = 0;
        }
        switch[numSwitches]: Switch {
            address = index + 1;
        }
        sink: Host {
            address = parent.numSwitches + 1;
        }
        routeManager: GlobalRouteManager;
   connections:
       sender.port++ <--> C <--> switch[0].port++;
       sink.port++ <--> C <--> switch[numSwitches-1].port++;
       for i=0..numSwitches-2 {
           switch[i].port++ <--> C <--> switch[i+1].port++;
       }
           
}