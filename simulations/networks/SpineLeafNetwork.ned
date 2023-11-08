package simnet.simulations.networks;
import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.TrafficPatternManager;
import simnet.mod.manager.GlobalView;
import simnet.mod.manager.GlobalGroupManager;

network SpineLeafNetwork
{
    parameters:
        @display("bgb=1152.1562,432.23752");
        int spineNumber;
        int leafNumber;
        int hostNumber;
        double fabricLinkBW @unit(bps) = default(40Gbps);
        double fabricLinkDealy @unit(s) = default(1us);
        double edgeLinkBW @unit(bps) = default(10Gbps);
        double edgeLinkDelay @unit(s) = default(1us);
        @statistic[fct](title="flow complete time"; source=fct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[idealFct](title="ideal flow complete time"; source=idealFct; unit=s; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[flowSize](title="flow total bytes"; source=flowSize; unit=B; record=count,mean,max,min,vector,timeavg; interpolationmode=none);
        @statistic[jobRCT](record=vector; source=jobRCT; interpolationmode=none);

    types:
        channel FabricChannel extends DatarateChannel
        {
            datarate = parent.fabricLinkBW;
            delay = parent.edgeLinkDelay;
        }
        channel EdgeChannel extends DatarateChannel
        {
            datarate = parent.edgeLinkBW;
            delay = parent.edgeLinkDelay;
        }

    submodules:
        hosts[leafNumber*hostNumber]: Host {
            @display("p=200,350,r,50");
        }
        leafSwitches[leafNumber]: Switch {
            @display("p=200,200,r,100");
        }
        spineSwitches[spineNumber]: Switch {
            @display("p=200,100,r,150");
        }
        routeManager: GlobalRouteManager {
            @display("p=957.5063,37.2125");
        }
        tpManager: TrafficPatternManager {
            @display("p=1078,38");
        }
        globalView: GlobalView {
            @display("p=957.5063,143.125");
        }
        groupManager: GlobalGroupManager {
            @display("p=1077.7313,143.125");
        }
    connections:
        for i=0..leafNumber-1, for j=0..spineNumber-1 {
            leafSwitches[i].port++ <--> FabricChannel <--> spineSwitches[j].port++;
        }
        for i=0..hostNumber-1, for j=0..leafNumber-1 {
            hosts[i + j*hostNumber].port++ <--> EdgeChannel <--> leafSwitches[j].port++;
        }
}

