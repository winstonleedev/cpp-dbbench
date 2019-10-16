cmake_minimum_required(VERSION 3.10)
project(DBBench)

set(CMAKE_CXX_STANDARD 17)

find_library(LEVELDB_PATH NAMES leveldb PATHS /usr)
find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

include(${CMAKE_CURRENT_SOURCE_DIR}/wiredis/CMakeLists.txt)

include_directories(
    ${BOOST_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/wiredis/include
)

file(GLOB ALLSRC
    "*.h"
    "*.cpp"
    "ledger/*.h"
    "ledger/*.cpp"
)

add_executable(DBBench ${ALLSRC})

target_link_libraries(DBBench
PUBLIC
    ${LEVELDB_PATH}
    ${BOOST_LIBRARIES}
    Threads::Threads
)