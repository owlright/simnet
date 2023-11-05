#include "ModuleAccess.h"

cModule* getRemoteModule(const cModule* from, const char* gatename, int index)
{
    auto node = from->getParentModule();
    if (node->gate(gatename)->getType() == cGate::INPUT) {
        auto source_gate = node->gate(gatename, index)->getIncomingTransmissionChannel()->getSourceGate();
        auto source_gate_ = source_gate->getPreviousGate();
        while(source_gate_) {
            source_gate = source_gate_;
            source_gate_ = source_gate->getPreviousGate();
        }
        return source_gate->getOwnerModule()->getParentModule();
    }
    else {
        auto dest_gate = node->gate(gatename, index)->getTransmissionChannel()->getSourceGate();
        auto dest_gate_ = dest_gate->getNextGate();
        while(dest_gate_) {
            dest_gate = dest_gate_;
            dest_gate_ = dest_gate->getNextGate();
        }
        return dest_gate->getOwnerModule()->getParentModule();
    }
}