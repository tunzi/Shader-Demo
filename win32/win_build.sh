#!/bin/bash
#LD_LIBRARY_PATH="/home/xavierz3/cross-tools/lib/SDL" i386-mingw32-c++ julia.cpp -lSDL -lm -lstdc++ -o julia.exe

BASE="shader"

CC=i386-mingw32-c++

#commented because of multiple .cpp files
#BASE=$(basename ${1-$(find|grep '.cpp$')} .cpp)
BASE="shader"

ARGS='-L /home/xavierz3/cross-tools/lib/SDL -lSDL -lm -lstdc++'

OUT="-o ${BASE}.exe"

$CC $(ls|grep 'cpp$'|xargs) $ARGS $OUT

#full command:
#i386-mingw32-c++ `basename $1 .cpp`.cpp -L /home/xavierz3/cross-tools/lib/SDL -lSDL -lm -lstdc++ -o `basename $1 .cpp`.exe