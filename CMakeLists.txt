cmake_minimum_required(VERSION 3.10)
project(dbbench)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

include(GNUInstallDirs)

find_library(LEVELDB_PATH NAMES leveldb PATHS /usr)
find_library(ROCKSDB_PATH NAMES rocksdb PATHS /usr)

find_package(Threads REQUIRED)
find_package(Boost COMPONENTS system program_options REQUIRED)
find_package(hiredis REQUIRED)

message(STATUS "Boost libs: ${Boost_LIBRARIES}")
message(STATUS "Boost dirs: ${Boost_INCLUDE_DIR}")
message(STATUS "ROCKSDB_PATH: ${ROCKSDB_PATH}")

include_directories(
    ${Boost_INCLUDE_DIR}
    ${HIREDIS_INCLUDE_DIRS}
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

message(STATUS "CMAKE_DL_LIBS: ${CMAKE_DL_LIBS}")
target_link_libraries(${CMAKE_PROJECT_NAME}
PUBLIC
    ${LD_LIBRARY_PATH}
    ${CMAKE_DL_LIBS}
    Threads::Threads
    ${Boost_LIBRARIES}
    ${HIREDIS_LIBRARIES}
    ${LEVELDB_PATH}
    ${ROCKSDB_PATH}
)

configure_file(all.sh all.sh COPYONLY)