package node;

simple Controller
{
    parameters:
        string name=fullName();
        object routes = [
            { sources: "2,3", target: "1" },
            ];
        @display("i=block/cogwheel");
}