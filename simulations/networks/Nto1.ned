package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.Controller;
network Nto1
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
            address = 1234;
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

