cmake_minimum_required(VERSION 3.10)
project(TemporalGraph)

set(CMAKE_CXX_STANDARD 17)

include_directories(include)

add_executable(TemporalGraph
    src/main.cpp
    src/temporalGraph.cpp
    src/scoresComputation.cpp
    src/utils.cpp
)
