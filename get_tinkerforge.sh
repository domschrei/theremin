#!/bin/bash

set -e

URL_PATH='http://download.tinkerforge.com/bindings/c/'
FILE_NAME='tinkerforge_c_bindings_2_1_12.zip'

wget "$URL_PATH""$FILE_NAME"
mkdir -p tinkerforge
unzip "$FILE_NAME" -d tinkerforge/
cd tinkerforge/source/
make
cd ../..
rm "$FILE_NAME"

echo "Successfully fetched and built the Tinkerforge libraries."
