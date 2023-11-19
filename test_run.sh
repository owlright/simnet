#!/bin/bash

OPP_RUN_OPTIONS="-m --cmdenv-redirect-output=false \
--cmdenv-log-level=off \
--cmdenv-performance-display=false \
--cmdenv-event-banners=false \
-n simulations \
-n src/simnet -l src/simnet"

make
opp_run simulations/exp/omnetpp.ini -c fatTree1024loadbalancepermutation -u Cmdenv ${OPP_RUN_OPTIONS} -r '$load=0.1 && $aggPolicy="sptree" && $epsion=0.0 && $repetition=0'

#-r '$load=0.5 && $epsion=0.0 && $repetition=2' #-s -r 0 --sim-time-limit=0s
