//
// Created by thanhphu on 19. 11. 8..
//

#include "RandomEngine.h"

RandomEngine::RandomEngine(int keyLength, int valueLength) {
    uniform_dist_key = std::uniform_int_distribution<int>(0, keyLength);
    uniform_dist_value = std::uniform_int_distribution<int>(0, valueLength);
}

int RandomEngine::randKeys() {
    return (uniform_dist_key(r));
}

int RandomEngine::randValues() {
    return (uniform_dist_value(r));
}