#!/bin/sh

echo "Non-public functions used in other files:"
for f in src/*.c; do
    privs=`perl -n \
        -e '/^public / && ($p = 0);' \
        -e '/;/ && ($p = 0);' \
        -e '/^([a-z][a-zA-Z0-9_]*) *\(/           && $p > 1 && print "$1:$ln\n";' \
        -e '/^[^ #\/].* ([a-z][a-zA-Z0-9_]*) *\(/ && $p > 1 && print "$1:$ln\n";' \
        -e '$p++; $ln++;' \
        $f`
    for priv in $privs; do
        name=`echo $priv | cut -d : -f 1`
        ln=`echo $priv | cut -d : -f 2`
        grep --color=always -n "$name" src/*.c --exclude $f \
            && echo "  ...defined in $f:$ln"
    done
done
