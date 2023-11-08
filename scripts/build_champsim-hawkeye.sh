#!/bin/bash

./ChampSim/config.sh ./champsim-configs/hawkeye-config.json --replacement-dir ./hawkeye --bindir ./bin

cd ./ChampSim/ && make -j12
