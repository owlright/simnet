package simnet.app;

module SRWorker extends UnicastSenderApp {
parameters:
    port = default(2000);
    destPort = default(2001);
    int jobId           = default(-1);
    int workerId        = default(-1);
    int numWorkers      = default(0);
    string segmentAddrs @mutable = default("");
    string fanIndegrees @mutable = default("");
    @class(SRWorker);
}
