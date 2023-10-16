package simnet.simulations.networks;

import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalView;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;
import simnet.mod.manager.TrafficPatternManager;
import simnet.mod.manager.GlobalMetricCollector;

//channel C extends DatarateChannel
//{
//    delay = default(1us);
//    datarate = default(40Gbps);
//}

// There are k pods, each containing two layers of k/2 switches.
// Each k-port switch in the lower layer is directly connected to k/2 hosts.
// Each of the remaining k/2 ports is connected to k/2 of the k ports in the aggregation layer of the hierarchy.
network FatTree
{
    parameters:
        int k = default(64);
        int halfk = int(k/2);
        int numPods = k;
        int numCores = int(k^2/4);
        @display("bgb=1069.4512,303.225");
        @statistic[fct](title="flow complete time"; source=fct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[idealFct](title="ideal flow complete time"; source=idealFct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[flowSize](title="flow total bytes"; source=flowSize; unit=B; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[jobRCT](record=vector; source=jobRCT; interpolationmode=none);

    types:
        channel C extends DatarateChannel
        {
            delay = default(1us);
            datarate = default(40Gbps);
        }

    submodules:
        pods[numPods]: Pod {
            k = parent.k;
            @display("p=166,112,r,50");
        }
        coreSwitches[numCores]: Switch {
            @display("p=166,32,r,50");
        }
        routeManager: GlobalRouteManager {
            @display("p=803.5462,96.798744");
        }
        groupManager: GlobalGroupManager {
            @display("p=919.00494,96.798744");
        }
        tpManager: TrafficPatternManager {
            @display("p=803.5462,191.265");
        }
        metricCollector: GlobalMetricCollector {
            @display("p=919.00494,191.265");
        }
        globalView: GlobalView {
            @display("p=699.75,96.798744");
        }

    connections:
        for i=0..k-1, for j=0..halfk-1, for z=0..halfk-1 {
            pods[i].port[j+z*halfk] <--> C <--> coreSwitches[j+z*halfk].port++;
        }
}

module Pod
{
    parameters:
        int k; // port number of switch
        int hostNumber = int(k^2/4);
        int switchLevelNumber = int(k/2);
        int halfPorts = int(k/2);
        @display("bgb=778,400");

    gates:
        inout port[halfPorts*switchLevelNumber];

    types:
        channel C extends DatarateChannel
        {
            delay = default(1us);
            datarate = default(40Gbps);
        }

    submodules:
        level1[switchLevelNumber]: Switch {
            @display("p=101,50,r,100");
        }
        level0[switchLevelNumber]: Switch {
            @display("p=101,150,r,100");
        }
        hosts[hostNumber]: Host {
            @display("p=78,250,r,50");
        }
    connections allowunconnected:
        for i=0..halfPorts-1, for j=0..switchLevelNumber-1 {
            port[j*halfPorts+i] <--> level1[j].port++;
        }
        for i=0..switchLevelNumber-1, for j=0..halfPorts-1 {
            level0[i].port++ <--> C <--> hosts[i*halfPorts + j].port++;
        }
        for i=0..switchLevelNumber-1, for j=0..switchLevelNumber-1 {
            level0[i].port++ <--> C <--> level1[j].port++;
        }

}
