package mnet.app;

simple UnicastSenderApp like IApp {
    parameters:
        int address;  // local node address
        int port;
        int destAddress;
        int destPort;
        int messageLength @unit(B) = default(1000B);
        int flowSize @unit(MB) = default(10MB);
        int flowInterval @unit(s) = default(1s);
    gates:
        input in;
        output out;
}

simple UnicastEchoApp like IApp{
    parameters:
        int address;  // local node address
        int port;
    gates:
        input in;
        output out;
}