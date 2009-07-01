#!/bin/bash
rm -rf /tmp/dmtx*
killall dmtxd
mkdir /tmp/dmtxdatadir
mkdir /tmp/dmtxsymboldir
mkdir /tmp/dmtxoutputdir
./src/dmtxd &
#cat /tmp/dmtxdatadir/dmtxd.lock
cat /tmp/dmtxdatadir/dmtxd.log
ps aux | grep dmtxd
#cp sample-symbol.png /tmp/dmtxsymboldir/

cat /tmp/dmtxdatadir/dmtxd.log
ps aux | grep dmtxd
#cat /tmp/dmtxoutputdir/symbol.txt


