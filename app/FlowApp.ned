package range.app;
import range.mod.Socket;
module FlowApp like IApp
{
    parameters:
        int address;
        int destAddress = default(-1);
//        string destAddress = default(""); // list of IP addresses, separated by spaces ("": don't send)
        int flowLength @unit(B) = default(0B);
        int messageLength @unit(B) = default(1000B); // length of messages to generate, in bytes
        
        // work on interval mode by default
        volatile double arrivalInterval @unit(s) = default(1s);

        // work on load mode if load == 0
        double load = default(0);
        int flowLengthMean @unit(B) = default(0B);
        @class(FlowApp);

    gates:
        input in;
        output out;
        input socketIn @directIn;

   	submodules:
        socket: Socket;

    connections allowunconnected:
        in --> socket.in;
		out <-- socket.out;

}
