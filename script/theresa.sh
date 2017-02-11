#!/bin/bash

# Change this path if neccessary
theresa_path=/home/pi/theresa

cd "$theresa_path"
echo >> theresa_log
date >> theresa_log
LD_LIBRARY_PATH=tinkerforge/source/:/usr/local/lib/ ./theresa >> theresa_log 2>&1 &

amixer sset 'PCM' 95%
