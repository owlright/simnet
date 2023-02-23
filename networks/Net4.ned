package networks;

import node.Node;
import ned.DatarateChannel;

network Net4
{
    types:
        channel C extends DatarateChannel
        {
            delay = 1ms;
            datarate = 10Mbps;
        }
    submodules:
        rte[4]: Node {
            address = index;
        }
    connections:
        rte[1].port++ <--> C <--> rte[0].port++;
        rte[2].port++ <--> C <--> rte[0].port++;
        rte[3].port++ <--> C <--> rte[0].port++;
}

