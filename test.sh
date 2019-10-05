#!/bin/sh

# Usage: ./test.sh

[ -z "$CXX" ] && CXX=cc
CXXFLAGS="-Wall -Wextra -g -Isrc -DENTRY=test_main -DTEST -DLINUX $CXXFLAGS"
LDFLAGS=""

cmd="$CXX $CXXFLAGS src/main.c $LDFLAGS -o test"
echo $cmd
$cmd && ./test
