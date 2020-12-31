#!/bin/bash

set -e

# Check what to build
if [[ "$1" == "--gui" ]]; then
    export THEREMIN_GUI=true
elif [[ "$1" == "--cli" ]]; then
    unset THEREMIN_GUI
else
    echo "Please specify whether you wish to build Theresa with a graphical interface (option --gui) or with a command-line interface (option --cli)."
    exit 0
fi

# CMake build
mkdir -p build
cd build
cmake ..
make

# Copy executables
cp theresa ../
cd ..
if [ -z $THEREMIN_GUI ]; then
    cp theresa theresa_cli
else
    cp theresa theresa_gui
fi

# Feedback
echo
echo "Build successful. Execute <./theresa> to launch the application."
echo "Edit the file <theresa.cfg> to configure the application."
