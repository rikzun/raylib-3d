#!/bin/bash

WORKSPACE=$(dirname "$(realpath $0)");
OUTPUT_FOLDER=$WORKSPACE/output
OUTPUT_FILE=$OUTPUT_FOLDER/debug-application.exe
SOURCE_FOLDER=$WORKSPACE/src
INPUT_FILE=$SOURCE_FOLDER/main.cpp

cd C:/w64devkit/bin/;
mkdir -p $OUTPUT_FOLDER;
gcc -o $OUTPUT_FILE $INPUT_FILE -lraylib -lgdi32 -lwinmm