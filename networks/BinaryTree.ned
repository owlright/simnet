package range.networks;

import range.mod.Host;
import ned.DatarateChannel;
import range.mod.Controller;

network BinaryTree
{
    parameters:
        string globalController = fullName()+".controller";
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
            globalController = parent.globalController;
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
