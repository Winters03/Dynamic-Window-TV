#!/bin/bash

cd build 2>/dev/null
if [ $? -ne 0 ]; then
    echo "REBUILDING!"
    rm -rf build
    mkdir build
    cd build
fi

if [ "$1" = "Win32" ]; then
    echo "Creating Windows Executable."
    cmake ..
    cmake --build . --config Release
    cd ..
    cp ./build/Release/my_app.exe ./my_app.exe
elif [ "$1" = "MacOS" ]; then
    echo "Running MacOS Executable."
    cmake ..
    make
    cd ..
    cp ./build/my_app ./my_app
    #echo $(pwd)
    ./my_app
fi