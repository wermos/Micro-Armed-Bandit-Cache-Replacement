#!/bin/bash

./ChampSim/config.sh --replacement-dir ./rlr --bindir ./bin "$@"

cd ./ChampSim/ && make -j12
