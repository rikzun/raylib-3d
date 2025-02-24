#!/bin/bash

WORKSPACE=$(dirname "$(realpath $0)")
DEVKIT=$(dirname "$(realpath $1)")

OUTPUT_FOLDER="$WORKSPACE/output"
OUTPUT_FILE="$OUTPUT_FOLDER/mod.dll"
HEADERS_FOLDER="$WORKSPACE/includes"
SOURCE_FILE="$WORKSPACE/mod.cpp"

mkdir -p $OUTPUT_FOLDER

"$DEVKIT/g++" \
    -B "$DEVKIT" \
    -I "$HEADERS_FOLDER" \
    -o "$OUTPUT_FILE" \
    $SOURCE_FILE \
    -shared \
    -fPIC \
    -std=c++2b \
    -O2 \
    -Wl,--gc-sections \
    -g0 \
    -s