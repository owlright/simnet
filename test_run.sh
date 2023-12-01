#!/bin/bash

OPP_RUN_OPTIONS="-m --cmdenv-redirect-output=false \
--cmdenv-log-level=off \
--cmdenv-performance-display=false \
--cmdenv-event-banners=false \
--cmdenv-redirect-output=false \
-n simulations \
-n src/simnet -l src/simnet"

make MODE=debug
if [ $? -eq 0 ]; then
    opp_run_dbg simulations/exp/omnetpp.ini -c fatTreeLoadbalance -u Cmdenv ${OPP_RUN_OPTIONS} --sim-time-limit=1s -s -r 12
else
    echo "Exit because make failed"
fi

