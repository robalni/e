#!/bin/sh

# Usage: ./test.sh

[ -z "$CXX" ] && CXX=c++
CXXFLAGS="-Wall -Wextra -g -Isrc -DENTRY=test_main -DTEST -DLINUX $CXXFLAGS"
LDFLAGS=""

cmd="$CXX $CXXFLAGS all.cpp $LDFLAGS -o test"
echo $cmd
$cmd && ./test
