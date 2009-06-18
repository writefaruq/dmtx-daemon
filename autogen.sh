#!/bin/sh
echo "Running aclocal"
aclocal
echo "Running automake"
automake --add-missing
echo "Running autoconf"
autoconf
echo "Running configure"
./configure
