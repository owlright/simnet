package simnet.simulations.networks;

import simnet.mod.Host;
import ned.DatarateChannel;
import simnet.mod.Controller;

network BinaryTree
{
    parameters:
        string globalRouteManager = fullName()+".controller";
        int height;
    types:
        channel C extends DatarateChannel
        {
            delay = 1ms;
            datarate = 10Mbps;
        }
    submodules:
        terminal[2^height]: Host {
            address = index;
            globalRouteManager = parent.globalRouteManager;
        }
        controller: Controller {
            @display("p=89,26;is=s");
        }
    connections allowunconnected:
        for i=0..2^(height-1)-2 {
            terminal[i].port++ <--> C <--> terminal[2*i+1].port++;
            terminal[i].port++ <--> C <--> terminal[2*i+2].port++;
        }
        terminal[0].port++ <--> C <--> terminal[7].port++;
}
