cmake_minimum_required(VERSION 3.15)
project(DBBench)

set(CMAKE_CXX_STANDARD 17)

file(GLOB ALLSRC
    "*.h"
    "*.cpp"
)

add_executable(DBBench ${ALLSRC})