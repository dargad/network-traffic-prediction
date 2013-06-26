cmake_minimum_required(VERSION 2.8)
project(NetworkTrafficPrediction)

include_directories(${CMAKE_SOURCE_DIR}/models/include)
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS program_options serialization system thread)
find_package(Threads REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

add_subdirectory(models)
add_subdirectory(server)
add_subdirectory(client)
