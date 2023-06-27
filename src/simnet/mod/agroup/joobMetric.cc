#include "jobMetric.h"

jobMetric::jobMetric(cModule* fromModule, IntAddress group)
{
    module = fromModule;
    jobId = group;
}

void jobMetric::createBufferSignalForGroup(IntAddress group)
{
    char signalName[32];
    sprintf(signalName, "group%" PRId64 "-usedBuffer", group);
    simsignal_t signal = module->registerSignal(signalName);

    char statisticName[32];
    sprintf(statisticName, "group%" PRId64 "usedBuffer", group);
    cProperty *statisticTemplate =
        module->getProperties()->get("statisticTemplate", "groupUsedBuffer");
    getEnvir()->addResultRecorders(module, signal, statisticName, statisticTemplate);
    usedBufferSignal = signal;
}

void jobMetric::emitAllSignals()
{
    module->emit(usedBufferSignal, usedBuffer);
}
