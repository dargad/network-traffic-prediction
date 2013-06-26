set(SRCS
    src/predictionserver.cpp
    src/main.cpp
)

include_directories(
    include
)

add_executable(server ${SRCS})
target_link_libraries(server
    models
    ${Boost_THREAD_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_SERIALIZATION_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${CMAKE_THREAD_LIBS_INIT}
)
