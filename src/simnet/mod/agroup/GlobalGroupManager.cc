#include "GlobalGroupManager.h"

Define_Module(GlobalGroupManager);

AggGroupEntry *GlobalGroupManager::getGroupEntry(IntAddress group)
{
    auto entry = new AggGroupEntry();
    return entry;
}
