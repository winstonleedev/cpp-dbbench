//
// Created by thanhphu on 19. 11. 4..
//

#pragma once

#ifndef DBBENCH_UTIL_H
#define DBBENCH_UTIL_H

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATE_DB        _ENV("PATH_STATE_DB", "/home/medium/medium/data/states-")

// Data structures

struct options {
    int dbType;
    int readWeight;
    int writeWeight;
    int duration;
    int stringLength;
    int keys;
    unsigned int threads;
};

long treeSize(const fs::path& pathToShow);

#endif //DBBENCH_UTIL_H
