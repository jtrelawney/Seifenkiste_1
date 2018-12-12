#!/bin/bash

cd lib/build
cmake ..
make install
cd ../../PC_cockpit/build
cmake ..
make

