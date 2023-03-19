package range.networks;

import range.mod.Node;
import ned.DatarateChannel;
import range.mod.Controller;
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
        sender[numberOfSenders]: Node {
            @node;
            address = index;
            app.destAddress = parent.numberOfSenders;
            globalController = parent.globalController;
        }
        gate: Node {
            @node;
            address = 1111;
            globalController = parent.globalController;
        }
        sink: Node {
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

