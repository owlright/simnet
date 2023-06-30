package simnet.simulations.test;
import simnet.mod.Host;
import simnet.mod.Switch;
import ned.DatarateChannel;
import simnet.mod.manager.GlobalRouteManager;
import simnet.mod.manager.GlobalGroupManager;
import simnet.mod.manager.TrafficPatternManager;

channel C extends DatarateChannel
{
    delay = 1us;
    datarate = 40Gbps;
}

network MiniFatTree
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