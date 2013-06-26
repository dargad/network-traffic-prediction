EXEC=Debug/client
export LD_LIBRARY_PATH=../models/Release:${LD_LIBRARY_PATH}
DATA_FILE=/home/darek/output
DEBUG_LEVEL=1
MODE=training
HORIZON=1
INPUT_NET=/home/darek/workspace-cdt/testrun/learning.net
RESULT_FILE=/home/darek/workspace-cdt/client/result-h${HORIZON}.txt
DO=0 # DATA_OFFSET
DL=2000 # DATA_LENGTH
STEP=10 # PREDICTION STEP
SERVER1=localhost:4421
SERVER2=localhost:4422
SERVER3=localhost:4423

echo ${EXEC} -H ${HORIZON} -f ${DATA_FILE} -m ${MODE} -d ${DEBUG_LEVEL} -i ${INPUT_NET} -r ${RESULT_FILE} --data-offset ${DO} --data-length ${DL} -s ${SERVER1} -s ${SERVER2} -s ${SERVER3} --prediction-step ${STEP}
${EXEC} -H ${HORIZON} -f ${DATA_FILE} -m ${MODE} -d ${DEBUG_LEVEL} -i ${INPUT_NET} -r ${RESULT_FILE} --data-offset ${DO} --data-length ${DL} -s ${SERVER1} -s ${SERVER2} -s ${SERVER3} --prediction-step ${STEP}
