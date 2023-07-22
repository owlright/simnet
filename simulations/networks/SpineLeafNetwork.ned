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
        @display("bgb=1141,347");
        int spineNumber;
        int podNumber;
        int hostNumber;
        double bandwidth @unit(bps);
        double linkDelay @unit(s);

//        @statistic[flowCompleteTime](title="flow complete time"; source=flowCompleteTime; record=count,mean,max,min,vector; interpolationmode=none);

    types:
        channel C extends DatarateChannel
        {
            datarate = parent.bandwidth;
            delay = parent.linkDelay;
        }

    submodules:
        pods[podNumber]: SpineLeafPod {
            @display("p=203,250,r,100");
            hostNumber = parent.hostNumber;
            podNumber = parent.podNumber;
        }
        coreSwitches[spineNumber]: Switch {
            @display("p=203,180,r,100");
        }
        routeManager: GlobalRouteManager {
            @display("p=998,38");
        }
        tpManager: TrafficPatternManager {
            @display("p=1078,38");
        }
        globalView: GlobalView {
            @display("p=998,107");
        }
        groupManager: GlobalGroupManager {

            @display("p=1078,107");
        }
    connections:
        for i=0..podNumber-1, for j=0..spineNumber-1 {
            pods[i].port++ <--> C <--> coreSwitches[j].port++;
        }
}

module SpineLeafPod
{
    parameters:
        @display("bgb=459,257");
        int hostNumber;
        int podNumber;
        double bandwidth @unit(bps);
        double linkDelay @unit(s);

    gates:
        inout port[0];

    types:
        channel C extends DatarateChannel
        {
            datarate = parent.bandwidth;
            delay = parent.linkDelay;
        }

    submodules:
        hosts[hostNumber]: Host {
            @display("p=50,150,r,50");
        }
        edgeSwitch: Switch {
            @display("p=50,80");
        }

    connections:
        for i=0..hostNumber-1 {
            hosts[i].port++ <--> C <--> edgeSwitch.port++;
        }
        for i=0..sizeof(port)-1 {
            port[i] <--> edgeSwitch.port++;
        }
}
