package simnet.simulations.networks;
import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.Controller;

network SpineLeafNetwork {
    parameters:
        @display("bgb=459,257");
        string globalController = fullName()+".controller";
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
        controller: Controller;
        pods[podNumber]: SpineLeafPod {
            @display("p=50,150,r,50");
            globalController = parent.globalController;
            hostNumber = parent.hostNumber;
            podNumber = parent.podNumber;
        }
        switches[spineNumber]: Switch {
            address = index + 10000;
            globalController = parent.globalController;
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
        string globalController;
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
            globalController = parent.globalController;
            @display("p=50,150,r,50");
        }
        switch: Switch {
            globalController = parent.globalController;
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