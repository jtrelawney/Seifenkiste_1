#!/bin/bash

cd /home/ethan/projects/Seifenkiste_1/lib/build
cmake ..
make install
cd /home/ethan/projects/Seifenkiste_1/PC_cockpit/build
cmake ..
make

