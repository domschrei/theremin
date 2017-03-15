#!/bin/bash

set -e

# Minimal command without CMake
#g++ -std=c++11 "$@" -L./tinkerforge/source/ -lSDL2 -lSDL2_ttf -ltinkerforge src/wave_synth.cpp src/user_interface.cpp src/audio.cpp src/sensor_input.cpp src/configuration.cpp src/main.cpp -o theresa

# CMake build
mkdir -p build
cd build
cmake ..
make
cp theresa ../

echo
echo "Compilation successful. Execute the command"
echo "    ./theresa"
echo "to launch the application."
echo "Edit the file"
echo "    theresa.cfg"
echo "to configure the application."
