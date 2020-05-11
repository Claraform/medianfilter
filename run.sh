#!/bin/bash

ts=$(date +%s%N)
mpirun -np 24 bin/Prac3
echo "Total runtime:"
echo $((($(date +%s%N) - $ts)/1000000))
