set(SRCS
    src/outputwriter.cpp
    src/main.cpp
    src/neuralproxy.cpp
    src/modelproxy.cpp
    src/predictionclient.cpp
)

include_directories(
    include
)

add_executable(client ${SRCS})
target_link_libraries(client
    models
    ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    pthread
)
