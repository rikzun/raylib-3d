#!/bin/bash

WORKSPACE=$(dirname "$(realpath $0)")
DEVKIT=$(dirname "$(realpath $1)")

OUTPUT_FOLDER="$WORKSPACE/output"
OUTPUT_FILE="$OUTPUT_FOLDER/debug.exe"
HEADERS_FOLDER="$WORKSPACE/includes"
SOURCE_FOLDER="$WORKSPACE/src"
SOURCE_FILES=$(find $SOURCE_FOLDER -type f -name "*.cpp")

mkdir -p $OUTPUT_FOLDER

"$DEVKIT/g++" \
    -B "$DEVKIT" \
    -I "$HEADERS_FOLDER" \
    -o "$OUTPUT_FILE" \
    $SOURCE_FILES \
    -lraylib \
    -lgdi32 \
    -lwinmm \
    -std=c++2b \
    -O2 \
    -Wl,--gc-sections \
    -g0 \
    -s