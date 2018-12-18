#!/bin/bash

HN=$(hostname)
echo "starting PC cockpit on $HN"

#PLATFORM_EXE="build/"$HN"_cockpit"
PLATFORM_EXE="/home/ethan/projects/Seifenkiste_1/PC_cockpit/build/pc_cockpit_main"

//./$PLATFORM_EXE
$PLATFORM_EXE

echo "\n\n\nmain script done!!!!!\n\n\n"


