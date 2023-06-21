package simnet.app;

module SRWorker extends UnicastSenderApp {
parameters:
    port = default(2000);
    destPort = default(2001);
    int groupAddress = default(this.destAddress);
    @class(SRWorker);
}
