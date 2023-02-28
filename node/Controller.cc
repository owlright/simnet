//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
#include <omnetpp.h>
#include "Controller.h"
Define_Module(Controller);
Controller::Controller() {
    // TODO Auto-generated constructor stub

}

Controller::~Controller() {
    // TODO Auto-generated destructor stub
}

void Controller::initialize(int stage)
{
    if (stage == 0) {
        EV_INFO << "network intialization." << endl;
        assert(getParentModule()->getSubmoduleVectorSize("rte") > 0);
        cTopology *topo = new cTopology("topo");
        std::vector<std::string> nedTypes;
        nedTypes.push_back(getParentModule()->getSubmodule("rte", 0)->getNedTypeName());
        topo->extractByNedTypeName(nedTypes);
        EV << "cTopology found " << topo->getNumNodes() << " nodes\n";
        delete topo;
    }
}
