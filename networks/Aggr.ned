package range.networks;

import range.mod.Node;
import ned.DatarateChannel;
import range.mod.Controller;

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
        gateSink: Node {
            @node;
            address = 31;
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
        gate.port++ <--> C <--> gateSink.port++;
        gateSink.port++ <--> C <--> sink1.port++;
        gateSink.port++ <--> C <--> sink2.port++;
}

network AggrRace
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
        group2[8]: Node {
            @node;
            address = index+2;
            app.destAddress = 41;
            app.groupAddress = app.destAddress;
            globalController = parent.globalController;
        }
        gate: Node {
            @node;
            address = 30;
            globalController = parent.globalController;
        }
        gateSink: Node {
            @node;
            address = 31;
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
            group1[i].port++  <--> C <--> gate.port++;
        }
        for i=0..7 {
            group2[i].port++  <--> C <--> gate.port++;
        }
		gate.port++ <--> C <--> gateSink.port++;
        gateSink.port++ <--> C <-->sink1.port++;
        gateSink.port++ <--> C <-->sink2.port++;

}

network AggrNto1
{
     parameters:
        string globalController = fullName()+".controller";
        int numberOfSenders = default(2);

    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }
    submodules:
        sender[numberOfSenders]: Node {
            @node;
            address = index;
            app.destAddress = parent.numberOfSenders;
            app.groupAddress = app.destAddress;
            globalController = parent.globalController;
        }
        gate: Node {
            @node;
            address =666;
            globalController = parent.globalController;
        }
        sink: Node {
            @node;
            address = parent.numberOfSenders;
            globalController = parent.globalController;
        }
        controller: Controller;
    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1{
        	sender[i].port++ <--> C <--> gate.port++;
        }
}