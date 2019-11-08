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


// Function declaration

void condition_test(bool expr, const std::string& message);

void integrity_test(int dbType);

void integrity_test();

void full_test(options opts);

#endif //DBBENCH_TEST_H
