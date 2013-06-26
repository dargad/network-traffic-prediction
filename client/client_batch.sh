PROFILE=Debug
EXEC=${PROFILE}/client
export LD_LIBRARY_PATH=../models/${PROFILE}:${LD_LIBRARY_PATH}
HISTORICAL="30 50 70 100 500 2000 5000"
HORIZONS="1 3 5 10 20 30"
DEBUG_LEVEL=2
DATA_FILE=/home/darek/output-1s.txt
MODE=prediction
DO=0 # DATA_OFFSET
STEP=300 # PREDICTION STEP
INPUT_NET=/home/darek/workspace-cdt/testrun/main.net
SERVER1=localhost:4421
SERVER2=localhost:4422
SERVER3=localhost:4423

for DL in ${HISTORICAL}
do
    for HORIZON in ${HORIZONS}
    do
        echo "Horizon ${HORIZON} Length ${DL}"
        RESULT_FILE=/home/darek/workspace-cdt/client/output/result-h${HORIZON}-s${STEP}-d${DL}.txt
        # echo "Debug/client -H ${HORIZON} -f ${DATA_FILE} -m ${MODE} -d ${DEBUG_LEVEL} -i ${INPUT_NET} -r ${RESULT_FILE} --data-offset ${DO} --data-length ${DL} -l ${FL} -s ${SERVER1} -s ${SERVER2} -s ${SERVER3} --prediction-step ${STEP}"
        ${EXEC} -n 600 -H ${HORIZON} -f ${DATA_FILE} -m ${MODE} -d ${DEBUG_LEVEL} -i ${INPUT_NET} -r ${RESULT_FILE} --data-offset ${DO} --data-length ${DL} -s ${SERVER1} -s ${SERVER2} -s ${SERVER3} --prediction-step ${STEP}
    done
done
