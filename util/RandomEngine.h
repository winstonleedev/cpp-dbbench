//
// Created by thanhphu on 19. 11. 8..
//

#ifndef DBBENCH_RANDOMENGINE_H
#define DBBENCH_RANDOMENGINE_H

#include <random>

class RandomEngine {
private:
    // Seed with a real random value, if available
    std::random_device r{};
    std::uniform_int_distribution<int> uniform_dist_key;
    std::uniform_int_distribution<int> uniform_dist_100;

public:
    RandomEngine(int keyLength, int valueLength);
    int randKeys();
    int rand100();
};


#endif //DBBENCH_RANDOMENGINE_H
