package simnet.simulations.agg;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;

channel C extends DatarateChannel
{
    delay = 1us;
    datarate = 40Gbps;
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

network testFatTree
{
    parameters:
        @display("bgb=860,400");
    submodules:
        coreS0: Switch {
            address = 5000;
            @display("p=300,75");
        }
        coreS1: Switch {
            address = 5001;
            @display("p=450,75");
        }
        podS0[4]: Switch {
            address = 4000 + index;
            @display("p=220,172,m,2,80,80");
        }
        podS1[4]: Switch {
            address = 3000 + index;
            @display("p=450,172,m,2,80,80");
        }
        podH0[4]: Host {
            address = 1000 + index;
            @display("p=128,350,r,70");
        }
        podH1[4]: Host {
            address = 2000 + index;
            @display("p=407,350,r,70");
        }

        routeManager: GlobalRouteManager {
            @display("p=717,61");
        }
        groupManager: GlobalGroupManager {

            @display("p=717,127");
        }
    connections:
        podH0[0].port++ <--> C <--> podS0[2].port++;
        podH0[1].port++ <--> C <--> podS0[2].port++;
        podH0[2].port++ <--> C <--> podS0[3].port++;
        podH0[3].port++ <--> C <--> podS0[3].port++;

        podH1[0].port++ <--> C <--> podS1[2].port++;
        podH1[1].port++ <--> C <--> podS1[2].port++;
        podH1[2].port++ <--> C <--> podS1[3].port++;
        podH1[3].port++ <--> C <--> podS1[3].port++;

        podS0[0].port++ <--> C <--> podS0[2].port++;
        podS0[0].port++ <--> C <--> podS0[3].port++;
        podS0[1].port++ <--> C <--> podS0[2].port++;
        podS0[1].port++ <--> C <--> podS0[3].port++;

        podS1[0].port++ <--> C <--> podS1[2].port++;
        podS1[0].port++ <--> C <--> podS1[3].port++;
        podS1[1].port++ <--> C <--> podS1[2].port++;
        podS1[1].port++ <--> C <--> podS1[3].port++;

        podS0[0].port++ <--> C <--> coreS0.port++;
        podS0[0].port++ <--> C <--> coreS1.port++;
        podS0[1].port++ <--> C <--> coreS0.port++;
        podS0[1].port++ <--> C <--> coreS1.port++;

        podS1[0].port++ <--> C <--> coreS0.port++;
        podS1[0].port++ <--> C <--> coreS1.port++;
        podS1[1].port++ <--> C <--> coreS0.port++;
        podS1[1].port++ <--> C <--> coreS1.port++;
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
