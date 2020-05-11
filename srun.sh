#!/bin/bash

ts=$(date +%s%N)
./bin/Sequential
echo "Total runtime:"
echo $((($(date +%s%N) - $ts)/1000000))
