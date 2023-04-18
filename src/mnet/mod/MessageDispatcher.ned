package mnet.mod;


simple PortDispatcher
{
    parameters:
        @class(PortDispatcher);
        @display("i=block/classifier_s");
    gates:
        input localIn[];
        output localOut[];
        input in;
        output out;
}

