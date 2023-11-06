#!/bin/bash

./ChampSim/config.sh ./champsim-configs/rlr-config.json --replacement-dir ./rlr --bindir ./bin

cd ./ChampSim/ && make -j12
