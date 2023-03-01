package networks;

import node.Node;
import ned.DatarateChannel;
import node.Controller;
network Net4
{
    parameters:
        string globalController = fullName()+".controller";

    types:
        channel C extends DatarateChannel
        {
            delay = 1ms;
            datarate = 10Mbps;
        }
    submodules:
        terminal[4]: Node {
            address = index;
            globalController = parent.globalController;
        }
        controller: Controller {
            @display("p=125,18;is=s");
        }
    connections:
        terminal[1].port++ <--> C <--> terminal[0].port++;
        terminal[2].port++ <--> C <--> terminal[0].port++;
        terminal[3].port++ <--> C <--> terminal[0].port++;
}

