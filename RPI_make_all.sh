#!/bin/bash

cd /home/pi/projects/Seifenkiste_1/lib/build
cmake ..
make install
cd /home/pi/projects/Seifenkiste_1/RPI_cockpit/build
cmake ..
make

