#!/bin/bash

set -e

# Minimal command without CMake
#g++ -std=c++11 "$@" -L./tinkerforge/source/ -lSDL2 -lSDL2_ttf -ltinkerforge src/wave_synth.cpp src/user_interface.cpp src/audio.cpp src/sensor_input.cpp src/main.cpp -o theremin

# CMake build
mkdir -p build
cd build
cmake ..
make
cp theremin ../
echo
echo "Compilation successful. Execute the command"
echo "    ./theremin"
echo "to launch the application."
