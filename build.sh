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

[ -z "$CC" ] && CC=cc
CFLAGS="-Wall -Wextra -DLINUX $CFLAGS"
LDFLAGS="-lncurses $LDFLAGS"

if [ $debug ]; then
    CFLAGS="-g $CFLAGS"
else
    CFLAGS="-O2 $CFLAGS"
fi

if [ ! $nogui ]; then
    CFLAGS="`pkg-config --cflags xft` -DHAVE_X11 $CFLAGS"
    LDFLAGS="`pkg-config --libs x11` `pkg-config --libs xft` $LDFLAGS"
fi

cmd="$CC $CFLAGS src/editor.c $LDFLAGS -o e"
echo $cmd
$cmd
