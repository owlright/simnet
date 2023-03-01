package node;

simple Controller
{
    parameters:
        string name=fullName();
        object aggrgroup = [[1, 2],[2, 3]];
        object targets = [3, 1];
        @display("i=block/cogwheel");
}