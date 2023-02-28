package networks;

import node.Node;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Dumbbell
{
    parameters:
        int numberFlows = default(1);
    types:
        channel C extends DatarateChannel
        {
            delay = 100ms;
            datarate = 1Mbps;
        }

    submodules:
        sender[numberFlows]: Node {
            @display("p=,,c,40");
            address = index;
        }
        sink[numberFlows]: Node {
            @display("p=,,c,40");
            address = 10*parent.numberFlows + index;
        }
        LR: Node {
            @display("g=bottleneck;p=,,r,100");
            address = 1234;
        }
        RR: Node {
            @display("g=bottleneck;p=,,r,100");
            address = 4321;
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

