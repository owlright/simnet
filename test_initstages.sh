#!/bin/bash

OPP_RUN_OPTIONS="-m --cmdenv-redirect-output=false \
--cmdenv-log-level=off \
--cmdenv-performance-display=false \
--cmdenv-event-banners=false \
-n simulations \
-n src/simnet -l src/simnet"

make

opp_run simulations/exp/omnetpp.ini -c fatTree -u Cmdenv $OPP_RUN_OPTIONS  -s -r 0 #--sim-time-limit=0s
