#!/bin/sh

usage='./build.sh [debug] [nogui]'

while [ $# -gt 0 ]; do
    if [ "$1" = debug ]; then
        debug=1
    elif [ "$1" = nogui ]; then
        nogui=1
    else
        echo "Unknown option: $1"
        echo "Usage: $usage"
        exit 1
    fi
    shift
done

[ -z "$CXX" ] && CXX=c++
CXXFLAGS="-Wall -Wextra $CXXFLAGS"
LDFLAGS="-lncurses $LDFLAGS"

if [ $debug ]; then
    CXXFLAGS="-g $CXXFLAGS"
else
    CXXFLAGS="-O2 $CXXFLAGS"
fi

if [ ! $nogui ]; then
    CXXFLAGS="`pkg-config --cflags xft` -DHAVE_X11 $CXXFLAGS"
    LDFLAGS="`pkg-config --libs x11` `pkg-config --libs xft` $LDFLAGS"
fi

cmd="$CXX $CXXFLAGS all.cpp $LDFLAGS -o e"
echo $cmd
$cmd
