//
// Created by thanhphu on 19. 11. 4..
//

#pragma once

#ifndef DBBENCH_UTIL_H
#define DBBENCH_UTIL_H

#include <experimental/filesystem>
#include <random>

namespace fs = std::experimental::filesystem;

int rand10000();
int randBig();
long treeSize(const fs::path& pathToShow);

#endif //DBBENCH_UTIL_H
