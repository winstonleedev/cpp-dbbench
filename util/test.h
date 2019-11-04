//
// Created by thanhphu on 19. 11. 4..
//

#pragma once

#ifndef DBBENCH_TEST_H
#define DBBENCH_TEST_H

#include <iostream>
#include <chrono>

#include <util/util.h>

#include "ledger/db_provider.h"

#define _ENV(x, y)          (std::getenv(x) ? std::getenv(x) : y)
#define PATH_STATE_DB        _ENV("PATH_STATE_DB", "/home/medium/medium/data/states-")

// Data structures

struct options {
    int dbType;
    int readWeight;
    int writeWeight;
    int duration;
    int stringLength;
};

// Function declaration

void condition_test(bool expr, const std::string& message);

void integrity_test(int dbType);

void integrity_test();

void full_test(options opts);

#endif //DBBENCH_TEST_H
