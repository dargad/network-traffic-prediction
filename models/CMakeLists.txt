set(SRCS
    src/comm/protocol.cpp
    src/grey/grey.cpp
    src/util.cpp
    src/neural/netserializer.cpp
    src/neural/inputlayer.cpp
    src/neural/neuron.cpp
    src/neural/layer.cpp
    src/neural/neuralnet.cpp
    src/neural/outputlayer.cpp
    src/neural/activationfunction.cpp
    src/dataprovider/multidataprovider.cpp
    src/dataprovider/dataprovider.cpp
    src/arima/arima.cpp
    src/chaos/chaos.cpp
)

add_library(models ${SRCS})
