package simnet.simulations.networks;
import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;

network SpineLeafNetwork {
    parameters:
        @display("bgb=459,257");
        string globalRouteManager = fullName()+".routeManager";
        int spineNumber;
        int podNumber;
        int hostNumber;
        double bandwidth @unit(bps);
        double linkDelay @unit(s);

        @statistic[flowCompleteTime](title="flow complete time"; source=flowCompleteTime; record=count,mean,max,min,vector; interpolationmode=none);

    types:
        channel C extends DatarateChannel {
            datarate = parent.bandwidth;
            delay = parent.linkDelay;
        }

    submodules:
        routeManager: GlobalRouteManager;
        pods[podNumber]: SpineLeafPod {
            @display("p=50,150,r,50");
            globalRouteManager = parent.globalRouteManager;
            hostNumber = parent.hostNumber;
            podNumber = parent.podNumber;
        }
        switches[spineNumber]: Switch {
            address = index + 10000;
//            globalRouteManager = parent.globalRouteManager;
            @display("p=50,80,r,50");
        }

    connections:
        for i=0..podNumber-1, for j=0..spineNumber-1 {
            pods[i].port++ <--> C <--> switches[j].port++;
        }
}

module SpineLeafPod {
    parameters:
        @display("bgb=459,257");
        string globalRouteManager;
        int hostNumber;
        int podNumber;
        double bandwidth @unit(bps);
        double linkDelay @unit(s);

    gates:
        inout port[0];

    types:
        channel C extends DatarateChannel {
            datarate = parent.bandwidth;
            delay = parent.linkDelay;
        }

    submodules:
        hosts[hostNumber]: Host {
            address = parent.index * parent.hostNumber + index;
//            globalRouteManager = parent.globalRouteManager;
            @display("p=50,150,r,50");
        }
        switch: Switch {
//            globalRouteManager = parent.globalRouteManager;
            address = parent.index * parent.hostNumber + 1000;
            @display("p=50,80");
        }

    connections:
        for i=0..hostNumber-1 {
            hosts[i].port++ <--> C <--> switch.port++;
        }
        for i=0..sizeof(port)-1 {
            port[i] <--> switch.port++;
        }
}