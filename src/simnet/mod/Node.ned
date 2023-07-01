package simnet.mod;
import simnet.app.IApp;
import simnet.mod.contract.PortDispatcher;

module Node
{
    parameters:
        int address @mutable = default(-1);
        @class(Node);
        @node;

    gates:
        inout port[];

    submodules:
        routing: Routing {
            parameters:
                @display("p=140,163");
            gates:
                in[sizeof(parent.port)];
                out[sizeof(parent.port)];
        }
        queue[sizeof(port)]: L2Queue {
            parameters:
                @display("p=140,250,row");
        }

    connections:
        for i=0..sizeof(port)-1 {
            routing.out[i] --> queue[i].in;
            routing.in[i] <-- queue[i].out;
            queue[i].line <--> port[i];
        }
}

module Host extends Node
{
    parameters:
        double bandwidth @unit(bps) @mutable = default(0.0Gbps);
        int numApps = default(0);
        int numFlows = default(1);

        double load = default(0.0);
        int flowSizeMean @unit(B) = default(0B);
        double flowInterval @unit(s) = default(0s); // this means load = 1.0
        @host;
        @class(HostNode);
        @display("i=abstract/server_s;bgb=276,324");

    submodules:
        apps[numApps]: <default("UnicastSenderApp")> like IApp {
            @display("p=140,36.4,r,50");
        }
        at: PortDispatcher {
            parameters:
                @display("p=140,103");
            gates:
                localIn[sizeof(parent.apps)];
                localOut[sizeof(parent.apps)];
        }

    connections:
        for i=0..numApps-1 {
            apps[i].out --> at.localIn[i];
            apps[i].in <-- at.localOut[i];
        }
        at.out --> routing.localIn;
        at.in <-- routing.localOut;
}

module Switch extends Node
{
    parameters:
        int position = default(0); // the switch's level
        int numAggregators = default(40000);
        int agtrSize @unit(B) = default(264B);
        @switch;
        @class(SwitchNode);
        @display("i=abstract/switch_s");

    connections allowunconnected:
   	    // routing.localIn/localOut are not connected
}
