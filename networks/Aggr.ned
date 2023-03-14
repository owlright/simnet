package networks;
import ned.DatarateChannel;
import node.Controller;
import node.Node;

network Aggr
{
    parameters:
        string globalController = fullName()+".controller";

    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }

    submodules:
        controller: Controller;
        group1[2]: Node {
            @node;
            address = index;
            app.destAddress = 40;
            app.groupAddress = app.destAddress;
            globalController = parent.globalController;
        }
        aggr1: Node {
            @node;
            address = 10;
            globalController = parent.globalController;
        }
        group2[4]: Node {
            @node;
            address = index+2;
            app.destAddress = 41;
            app.groupAddress = app.destAddress;
            globalController = parent.globalController;
        }
        aggr2: Node {
            @node;
            address = 20;
            globalController = parent.globalController;
        }
        gate: Node {
            @node;
            address = 30;
            globalController = parent.globalController;
        }
        sink1: Node {
            @node;
            address = 40;
            globalController = parent.globalController;
        }
        sink2: Node {
            @node;
            address = 41;
            globalController = parent.globalController;
        }
    connections:
        for i=0..1 {
            group1[i].port++  <--> C <--> aggr1.port++;
        }
        for i=0..3 {
            group2[i].port++  <--> C <--> aggr2.port++;
        }
        aggr1.port++  <--> C <-->  gate.port++;
        aggr2.port++  <--> C <-->  gate.port++;
        gate.port++ <--> C <--> sink1.port++;
        gate.port++ <--> C <--> sink2.port++;
}