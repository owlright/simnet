package simnet.simulations.agg;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;

channel C extends DatarateChannel
{
    delay = 2us;
    datarate = 10Gbps;
}

network AggNto1
{
    parameters:
        int numberOfSenders = default(2);

    submodules:
        sender[numberOfSenders]: Host {
            address = index;
        }
        gate: Switch {
            address = 1234;
        }
        sink: Host {
            address = parent.numberOfSenders;
        }
        routeManager: GlobalRouteManager;
        groupManager: GlobalGroupManager;
    connections:
        gate.port++ <--> C <--> sink.port++;
        for i=0..numberOfSenders-1 {
            sender[i].port++ <--> C <--> gate.port++;
        }
}

network RTTVariance
{
    parameters:
        int g1numOfWorkers;
        int g2numOfWorkers;
        int numberOfbgflows;

    submodules:
        g1Worker[g1numOfWorkers]: Host {
            address = index + 1000;
        }
        g1PS: Host {
            address = parent.g1numOfWorkers + 1000;
        }
        g2Worker[g2numOfWorkers]: Host {
            address = index + 2000;
        }
        g2PS: Host {
            address = parent.g2numOfWorkers + 2000;
        }
        localSwitch: Switch {
            address = 1;
            @display("p=161,153");
        }
        midSwitch: Switch {
            address = 2;
            @display("p=285,153");
        }
        remoteSwitch: Switch {
            address = 3;
            @display("p=405,153");
        }
        source[numberOfbgflows]: Host {
            address = index + 3000;
            *.destAddress = default(index + 4000);
            @display("p=161,50");
        }
        sink[numberOfbgflows]: Host {
            address = index + 4000;
            @display("p=405,50");
        }
        routeManager: GlobalRouteManager {
            @display("p=598,38");
        }
        groupManager: GlobalGroupManager {
            @display("p=598,122");
        }
    connections:
        for i=0..g1numOfWorkers-1 {
            g1Worker[i].port++ <--> C <--> localSwitch.port++;
        }
        for i=0..g2numOfWorkers-1 {
            g2Worker[i].port++ <--> C <--> localSwitch.port++;
        }
        for i=0..numberOfbgflows-1 {
            source[i].port++ <--> C <--> localSwitch.port++;
        }
        for i=0..numberOfbgflows-1 {
            remoteSwitch.port++ <--> C <--> sink[i].port++;
        }
        localSwitch.port++ <--> C <--> midSwitch.port++;
        midSwitch.port++ <--> C <--> remoteSwitch.port++;
        g1PS.port++ <--> C <--> remoteSwitch.port++;
        g2PS.port++ <--> C <--> localSwitch.port++;
}

network MinFatTree
{
    parameters:
        int podNumber = 2;
        int coreNumber = 2;

    submodules:
        pods[podNumber]: Pod {
            @display("p=166,112,r,50");
        }
        coreSwitches[coreNumber]: Switch {
            address = 1000*index;
            @display("p=166,32,r,50");
        }

        routeManager: GlobalRouteManager;
        groupManager: GlobalGroupManager {

            @display("p=295,106");
        }
    connections:
        coreSwitches[0].port++ <--> C <--> pods[0].port[0];
        coreSwitches[1].port++ <--> C <--> pods[0].port[1];
        coreSwitches[0].port++ <--> C <--> pods[0].port[2];
        coreSwitches[1].port++ <--> C <--> pods[0].port[3];
        coreSwitches[0].port++ <--> C <--> pods[1].port[0];
        coreSwitches[1].port++ <--> C <--> pods[1].port[1];
        coreSwitches[0].port++ <--> C <--> pods[1].port[2];
        coreSwitches[1].port++ <--> C <--> pods[1].port[3];
}

module Pod
{
    parameters:
        int k = default(4); // port number of switch
        int hostNumber = int(k^2/4);
        int switchLevelNumber = int(k/2);
        int halfPorts = int(k/2);

        @display("bgb=400,400");
    gates:
        inout port[halfPorts*switchLevelNumber];

    submodules:        
        level1[switchLevelNumber]: Switch {
            address = parent.index*100 + (index) + 20;
            @display("p=101,50,r,100");
        }
        level0[switchLevelNumber]: Switch {
            address = parent.index*100 + (index) + 10;
            @display("p=101,150,r,100");
        }
        hosts[hostNumber]: Host {
            address = parent.index*100 + (index) + 30;
            @display("p=78,250,r,50");
        }
    connections:
        for i=0..switchLevelNumber-1, for j=0..halfPorts-1 {
            level0[i].port++ <--> C <--> hosts[i*halfPorts + j].port++;
        }
        for i=0..switchLevelNumber-1, for j=0..switchLevelNumber-1 {
            level0[i].port++ <--> C <--> level1[j].port++;
        }
        for i=0..halfPorts-1, for j=0..switchLevelNumber-1 {
            port[j*halfPorts+i] <--> level1[j].port++;
        }
}
