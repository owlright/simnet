package range.app;
import range.mod.Socket;
module FlowApp like IApp
{
    parameters:
        int address;
        int destAddress = default(-1);
        int groupAddress = default(-1);
        int packetLength @unit(B) = default(1000B);
        //        string destAddress = default(""); // list of IP addresses, separated by spaces ("": don't send)
        volatile int flowLength @unit(B) = default(0B);
//        int messageLength @unit(B) = default(1000B); // length of messages to generate, in bytes

        // work on interval mode by default
        volatile double arrivalInterval @unit(s) = default(1s);

        // work on load mode if load == 0
        double load = default(0);
        int flowLengthMean @unit(B) = default(0B);
        @class(FlowApp);
        @display("bgb=170,146");
        
        @signal[flowRealCompletionTime](type="simtime_t");
        @signal[flowIdealCompletionTime](type="simtime_t");
        @statistic[flowRealCompletionTime](title="realFCT";unit=s;record=vector,mean,max;interpolationmode=none);
        @statistic[flowIdealCompletionTime](title="idealFCT";unit=s;record=vector,mean,max;interpolationmode=none);
        
    gates:
        input in;
        output out;
        input socketIn @directIn;

    submodules:
        socket: Socket {

            @display("p=85,61");
        }
    connections allowunconnected:
        in --> socket.in;
        out <-- socket.out;

}
