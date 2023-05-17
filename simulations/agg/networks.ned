package simnet.simulations.agg;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;

network AggNto1
{
    parameters:
        int numberOfSenders = default(2);

    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }

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

    types:
        channel C extends DatarateChannel
        {
            delay = 2us;
            datarate = 10Gbps;
        }

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

