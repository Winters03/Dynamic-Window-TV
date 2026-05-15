#!/bin/bash

cd build
cmake ..
if [ $? -ne 0 ]; then
    echo "REBUILDING!"
    rm -rf build
    mkdir build
    #cd build
    cmake ..
fi
make
./my_app