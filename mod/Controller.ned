package range.mod;

simple Controller
{
    parameters:
        string name=fullName();
        object aggrouter = default([]);
        object aggrnumber = default([]);
        object aggrbuffer = default([]);
        object targets = default([]);
        @display("i=block/cogwheel");
}