#!/bin/bash

cd build 2>/dev/null
if [ $? -ne 0 ]; then
    echo "REBUILDING!"
    rm -rf build
    mkdir build
    cd build
fi

if [ "$1" = "Windows11" ]; then
    echo "Creating Windows Executable."
    cmake --build .
    make
elif [ "$1" = "MacOS" ]; then
    echo "Running MacOS Executable."
    cmake ..
    make
    ./my_app
fi

