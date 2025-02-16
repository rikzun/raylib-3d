#!/bin/bash

WORKSPACE=$(dirname "$(realpath $0)")
DEVKIT=$(dirname "$(realpath $1)")

OUTPUT_FOLDER=$WORKSPACE/output
OUTPUT_FILE=$OUTPUT_FOLDER/debug.exe
SOURCE_FOLDER=$WORKSPACE/src
INPUT_FILE=$SOURCE_FOLDER/main.cpp

mkdir -p $OUTPUT_FOLDER;
$DEVKIT/gcc -B $DEVKIT -o $OUTPUT_FILE $INPUT_FILE -lraylib -lgdi32 -lwinmm