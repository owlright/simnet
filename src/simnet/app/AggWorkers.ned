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
    double roundInterval @unit(us) = default(0us);
    double load = default(0.0);
    @class(WorkerApp);
}

module INCWorker extends WorkerApp {
parameters:
    string segmentAddrs @mutable = default("");
    string fanIndegrees @mutable = default("");
    int maxAgtrNum      = default(40000);
    @class(INCWorker);
}
