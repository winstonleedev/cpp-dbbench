//
// Created by thanhphu on 19. 11. 7..
//
#pragma once

#ifndef DBBENCH_WORKER_H
#define DBBENCH_WORKER_H

#include <iostream>
#include <atomic>

#include <util/util.h>
#include <ledger/state_db.h>
#include "RandomEngine.h"

class Worker {
public:
    Worker(struct options& _opts, avis::StateDB* _db, RandomEngine* _r, int _id);
    ~Worker();
    void run();
    unsigned long getReadCount();
    unsigned long getWriteCount();
    void stop();

private:
    // Input
    struct options opts{};
    avis::StateDB* db;
    RandomEngine* r;
    int id;

    // Output
    unsigned long readCount, writeCount;

    // Control
    std::atomic<bool> shouldStop = {};
    std::string* readResult;
    std::string long_string;
};


#endif //DBBENCH_WORKER_H
