#!/bin/bash

HN=$(hostname)
echo "starting RPI cockpit on $HN"

#PLATFORM_EXE="build/"$HN"_cockpit"
PLATFORM_EXE="build/rpi_cockpit_main"

./$PLATFORM_EXE


