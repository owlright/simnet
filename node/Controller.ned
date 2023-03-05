package node;

simple Controller
{
    parameters:
        string name=fullName();
        object aggrouter = default([[0]]);
        object aggrnumber = default([[2]]);
        object aggrbuffer = default([[0]]);
        object targets = default([1]);
        @display("i=block/cogwheel");
}