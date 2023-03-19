package range.app;
import range.mod.Socket;
module FlowApp like IApp
{
    parameters:
        int address;
        string destAddress = default(""); // list of IP addresses, separated by spaces ("": don't send)
        int flowLength @unit(B) = default(0B);
        int messageLength @unit(B) = default(1000B); // length of messages to generate, in bytes

    gates:
        input in;
        output out;
        input socketIn @directIn;

   	submodules:
        socket: Socket;

    connections:
        in --> socket.in;
		out <-- socket.out;

}
