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
LDFLAGS="-lncurses $LDFLAGS"

if [ $debug ]; then
    CXXFLAGS="-Wall -Wextra -g $CXXFLAGS"
else
    CXXFLAGS="-O2 $CXXFLAGS"
fi

if [ ! $nogui ]; then
    CXXFLAGS="-I/usr/include/freetype2 -DHAVE_X11 $CXXFLAGS"
    LDFLAGS="-lX11 -lfreetype -lxft $LDFLAGS"
fi

cmd="$CXX $CXXFLAGS all.cpp $LDFLAGS -o e"
echo $cmd
$cmd
