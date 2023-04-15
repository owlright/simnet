package mnet.networks;

import mnet.mod.Host;
import ned.DatarateChannel;
import mnet.mod.Controller;
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
            app.destAddress = parent.numberOfSenders;
            globalController = parent.globalController;
        }
        gate: Host {
            @node;
            address = 1111;
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

