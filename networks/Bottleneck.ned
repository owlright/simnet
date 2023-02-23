package networks;

import node.Node;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Bottleneck
{
    parameters:
        int numberFlows = default(1);
        double bandwidth @unit(Mbps) = default(2Mbps);
        double delay @unit(ms) = default(0.01ms);
        double access_bandwidth @unit(Mbps)= default(10Mbps);
        double access_delay @unit(ms) = default(45ms);
    types:
        channel accessLink extends DatarateChannel
        {
            delay = (parent.access_delay/1ms)*1e-3s;
            datarate = (parent.access_bandwidth/1Mbps)*1e6bps;
        }
        channel localLink extends DatarateChannel
        {
            delay = (parent.delay/1ms)*1e-3s;
            datarate = (parent.bandwidth/1Mbps)*1e6bps;
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
        gateway: Node {
            @display("g=bottleneck;p=,,r,100");
            address = 1234;
        }

    connections:
        for i=0..numberFlows-1 {
            sender[i].port++ <--> localLink <--> gateway.port++;
        }
        for i=0..numberFlows-1 {
            sink[i].port++ <--> accessLink <--> gateway.port++;
        }

}

