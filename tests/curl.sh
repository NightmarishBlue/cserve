#!/bin/sh
trap "trap - SIGTERM && kill -- -$$ &> /dev/null" SIGINT SIGTERM EXIT # ensure no orphans
set +e
PORT=6969
echo "check that curl localhost:$PORT works..."

curl "localhost:$PORT" &
CURL_PID=$!
"$EXE" $PORT & 
CSERVE_PID=$!
wait $CURL_PID
kill $CSERVE_PID
echo "test successful"