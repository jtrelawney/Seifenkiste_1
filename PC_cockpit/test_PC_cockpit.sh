#!/bin/bash

echo "start the PC cockpit test"

COCKPIT_TEST_EXE="build/pc_cockpit_test"
./$COCKPIT_TEST_EXE

echo "done testing the cockpit process"
