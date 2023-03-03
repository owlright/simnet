package networks;

import node.Node;
import ned.DatarateChannel;
import node.Controller;
network Nto1
{
    parameters:
        string globalController = fullName()+".controller";
        int numberOfSenders = default(2);

    types:
        channel C extends DatarateChannel
        {
            delay = 1ms;
            datarate = 10Mbps;
        }
    submodules:
        terminal[numberOfSenders+2]: Node {
            address = index;
            globalController = parent.globalController;
        }
        controller: Controller {
            @display("p=125,18;is=s");
        }
    connections:
        terminal[1].port++ <--> C <--> terminal[0].port++;
        for i=0..numberOfSenders-1 {
        	terminal[i+2].port++ <--> C <--> terminal[0].port++;
//        	terminal[i].port++ <--> C <--> terminal[0].port++;
        }
}

