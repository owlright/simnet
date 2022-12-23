package networks;

import node.Node;
import ned.DatarateChannel;
// one sender and one receiver
// a router with different datarate
network Net2
{
    types:
        channel CLarge extends DatarateChannel
        {
            delay = 100ms;
            datarate = 1Mbps;
        }
        channel Csmall extends DatarateChannel
        {
            delay = 100ms;
            datarate = 0.5Mbps;
        }

    submodules:
        rte[3]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> CLarge <--> rte[0].port++;
        rte[2].port++ <--> Csmall <--> rte[0].port++;
}

