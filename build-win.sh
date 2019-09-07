#!/bin/sh

usage='./build.sh [debug]'

while [ $# -gt 0 ]; do
    if [ "$1" = debug ]; then
        debug=1
    else
        echo "Unknown option: $1"
        echo "Usage: $usage"
        exit 1
    fi
    shift
done

[ -z "$CXX" ] && CXX=c++
CXXFLAGS="-Wall -Wextra -DWINDOWS -DHAVE_GUI $CXXFLAGS"
LDFLAGS="-lgdi32 $LDFLAGS"

if [ $debug ]; then
    CXXFLAGS="-g $CXXFLAGS"
else
    CXXFLAGS="-O2 $CXXFLAGS"
fi

cmd="$CXX $CXXFLAGS all.cpp $LDFLAGS -o e"
echo $cmd
$cmd
