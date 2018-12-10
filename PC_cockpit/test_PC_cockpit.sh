#!/bin/bash

echo "PC cockpit test "
echo "start the standard cockpit process" #and set it to stop after it has received N (100) images over TCP"


# tried to make it local
#HN=$(hostname)
#PLATFORM_EXE="build/"$HN"_cockpit"

PLATFORM_EXE="build/pc_cockpit_main"
./$PLATFORM_EXE 2>&1 &


echo "waiting for main script to catch up"
sleep 3s

read -n1 -r -p "Press space to continue..." key

echo "start camera process for the test..."

COCKPIT_TEST_EXE="build/pc_cockpit_test"
./$COCKPIT_TEST_EXE

echo "done testing the cockpit process"
