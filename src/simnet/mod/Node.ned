package simnet.mod;
import simnet.app.IApp;
import simnet.mod.contract.PortDispatcher;

moduleinterface INode
{
    parameters:
		int address;
		@node;
	gates:
        inout port[];
}

module Host like INode
{
    parameters:
        int address;
        int numApps = default(0);
        @node;
        @host;
        @display("i=abstract/server_s;bgb=276,324");
    gates:
        inout port[];
    submodules:
        apps[numApps]: <default("UnicastSenderApp")> like IApp {
            parameters:
                address = parent.address;
        }
        at: PortDispatcher {
            parameters:
                @display("p=140,103");
            gates:
                localIn[sizeof(parent.apps)];
                localOut[sizeof(parent.apps)];
        }
        routing: Routing {
            parameters:
                @display("p=140,161");
            gates:
                in[sizeof(parent.port)];
                out[sizeof(parent.port)];
        }
        queue[sizeof(port)]: L2Queue {
            parameters:
                @display("p=140,236,row");
        }
    connections:
        for i=0..numApps-1 {
            apps[i].out --> at.localIn[i];
            apps[i].in <-- at.localOut[i];
        }
        at.out --> routing.localIn;
        at.in <-- routing.localOut;
        for i=0..sizeof(port)-1 {
            routing.out[i] --> queue[i].in;
            routing.in[i] <-- queue[i].out;
            queue[i].line <--> port[i];
        }
}

module Switch like INode
{
    parameters:
        int address;
        @node;
        @switch;
        @display("i=abstract/switch_s");
    gates:
        inout port[];
    submodules:
        routing: Routing {
            parameters:
                @display("p=140,130");
            gates:
                in[sizeof(parent.port)];
                out[sizeof(parent.port)];
        }
        queue[sizeof(port)]: L2Queue {
            parameters:
                @display("p=80,200,row");
        }
    connections allowunconnected: // routing module's gates localIn/Out is not connected
        for i=0..sizeof(port)-1 {
            routing.out[i] --> queue[i].in;
            routing.in[i] <-- queue[i].out;
            queue[i].line <--> port[i];
        }
}
