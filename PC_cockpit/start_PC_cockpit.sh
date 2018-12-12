#!/bin/bash

HN=$(hostname)
echo "starting PC cockpit on $HN"

#PLATFORM_EXE="build/"$HN"_cockpit"
PLATFORM_EXE="build/pc_cockpit_main"

./$PLATFORM_EXE


