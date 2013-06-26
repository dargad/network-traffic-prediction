#!/bin/bash

PROFILE=Debug
EXEC=${PROFILE}/server
ALGORITHMS="chaos grey neural"
INPUT_FILE=output-1s.txt
PORT=4421
DEBUG_LEVEL=2
export LD_LIBRARY_PATH=../models/${PROFILE}:${LD_LIBRARY_PATH}

killall server -q

for alg in ${ALGORITHMS}
do
    ${EXEC} -a ${alg} -i ${INPUT_FILE} -l ${PORT} -d ${DEBUG_LEVEL} &
    (( PORT= $PORT + 1 ))
done
