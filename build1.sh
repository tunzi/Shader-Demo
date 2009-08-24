#!/bin/bash

CC=gcc

#commented because of multiple .cpp files
#BASE=$(basename ${1-$(find|grep '.cpp$')} .cpp)
BASE="shader"

ARGS=' -lSDL -lm -lstdc++'

OUT="-o "$BASE

$CC $(ls|grep 'cpp$'|xargs) $ARGS $OUT
