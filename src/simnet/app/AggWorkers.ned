package simnet.app;

module WorkerApp extends CongApp {
parameters:
    port = default(2000);
    destPort = default(3001);
    int flowSize @unit(B) = default(8kB);
    int numRounds = default(1);
    int jobId           = default(-1);
    int workerId        = default(-1);
    int numWorkers      = default(0);
    double roundInterval @unit(us) = default(1us);
    @class(WorkerApp);
}

module SRWorker extends WorkerApp {
parameters:
    string segmentAddrs @mutable = default("");
    string fanIndegrees @mutable = default("");
    @class(SRWorker);
}
