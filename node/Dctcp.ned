package node;

simple Dctcp like Cong
{
    parameters:
        double alpha = default(1.0);
        double g = default(0.0625);
        
        @class(TcpDctcp);
}