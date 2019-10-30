cmake_minimum_required(VERSION 3.10)
project(dbbench)

set(CMAKE_CXX_STANDARD 17)

include(GNUInstallDirs)

find_library(LEVELDB_PATH NAMES leveldb PATHS /usr)
find_library(ROCKSDB_PATH NAMES rocksdb PATHS /usr)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system program_options REQUIRED)

message(STATUS "Boost libs: ${Boost_LIBRARIES}")
message(STATUS "Boost dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "ROCKSDB_PATH: ${ROCKSDB_PATH}")

include_directories(
    ${Boost_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/wiredis/include
)

file(GLOB ALLSRC
    "*.cpp"
    "ledger/*.h"
    "ledger/*.cpp"
    "wiredis/include/wiredis/*.h"
    "wiredis/include/wiredis/proto/*.h"
    "lmdb/libraries/liblmdb/*.h"
    "lmdb/libraries/liblmdb/*.c"
)

add_executable(${CMAKE_PROJECT_NAME} ${ALLSRC})

target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    ${LD_LIBRARY_PATH}
    ${LEVELDB_PATH}
    ${ROCKSDB_PATH}
    ${Boost_LIBRARIES}
    Threads::Threads
)