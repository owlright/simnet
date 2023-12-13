#!/bin/bash

OPP_RUN_OPTIONS="-m --cmdenv-redirect-output=false \
--cmdenv-log-level=off \
--cmdenv-performance-display=false \
--cmdenv-event-banners=false \
--cmdenv-redirect-output=false \
-n simulations \
-n src/simnet -l src/simnet"

make
if [ $? -eq 0 ]; then
    opp_run simulations/exp/omnetpp.ini -c fatTreeWebEps -u Cmdenv ${OPP_RUN_OPTIONS} -r 0
else
    echo "Exit because make failed"
fi

# opp_run simulations/omnetpp.ini -c test -u Cmdenv -m --cmdenv-redirect-output=false --cmdenv-log-level=off --cmdenv-performance-display=false --cmdenv-event-banners=false -n simulations -n src/simnet -l src/simnet -r 0

# 查询Run和config对应关系
# opp_run simulations/fatTreeFbEps/omnetpp.ini -c fatTreeFbEps -q runs