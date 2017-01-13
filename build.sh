#!/bin/bash
g++ "$@" -O3 -L./tinkerforge/source/ -lSDL2 -lSDL2_ttf -ltinkerforge src/wave_synth.cpp src/input.cpp src/audio.cpp src/sensor_input.cpp src/main.cpp -o theremin
