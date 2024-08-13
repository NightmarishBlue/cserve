#!/bin/sh
trap "trap - SIGTERM && kill -- -$$ &> /dev/null" SIGINT SIGTERM EXIT # ensure no orphans
echo "-- check that running without arguments returns error..."

"$EXE"
[ $? -ne 2 ] && exit 1 # 2 is invalid argument in UNIX terms
echo "-- test passed"