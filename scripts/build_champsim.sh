#!/bin/bash

./ChampSim/config.sh --replacement-dir ./micro-armed-bandit --bindir ./bin "$@"

cd ./ChampSim/ && make -j12
