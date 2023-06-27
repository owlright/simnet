package simnet.simulations.networks;

import simnet.mod.Host;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;

network BinaryTree
{
    parameters:
        int height;
        
    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }
    submodules:
        V[2^height]: Host {
            address = index;
        }
		routeManager: GlobalRouteManager;
        groupManager: GlobalGroupManager;
    connections allowunconnected:
        for i=0..2^(height-1)-2 {
            V[i].port++ <--> C <--> V[2*i+1].port++;
            V[i].port++ <--> C <--> V[2*i+2].port++;
        }
        V[0].port++ <--> C <--> V[2^height-1].port++;
}
