#!/bin/bash

./ChampSim/config.sh "$@" --bindir ./bin

cd ./ChampSim/ && make -j12
