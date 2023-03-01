package node;

simple Controller
{
    parameters:
        string name=fullName();
        object aggrouter = [0];
        object aggrgroup = [2];
        object targets = [1];
        @display("i=block/cogwheel");
}