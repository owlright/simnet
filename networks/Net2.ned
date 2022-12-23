package networks;

import node.Node;
import ned.DatarateChannel;

network Net2
{
    types:
        channel C extends DatarateChannel
        {
            delay = 5ms;
            datarate = 1Mbps;
        }
    submodules:
        rte[2]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> C <--> rte[0].port++;
}

