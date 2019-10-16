#include <ledger/redis_db.h>

#include <iostream>

namespace avis {

RedisDB::~RedisDB() {
    close();
}

bool RedisDB::open(const std::string& dbname) {
    return true;
}

void RedisDB::close() {
}

bool RedisDB::opened() {
    return true;
}

bool RedisDB::get(const std::string& key, std::string* value) {
    return true;
}

bool RedisDB::put(const std::string& key, const std::string& value) {
    return true;
}

bool RedisDB::putBatch(const std::string& key, const std::string& value) {
    return true;
}

bool RedisDB::del(const std::string& key) {

    return true;
}

bool RedisDB::delBatch(const std::string& key) {
    return true;
}

bool RedisDB::applyBatch() {
    return true;
}

#ifndef FALLTHROUGH_INTENDED
#define FALLTHROUGH_INTENDED do { } while (0)
#endif

uint32_t RedisDB::hash(const char* data, size_t n, uint32_t seed) {

    // similar to murmur hash.
    const uint32_t m = 0xc6a4a793;
    const uint32_t r = 24;
    const char* limit = data + n;
    uint32_t       h = seed ^ (n * m);

    // pick up 4 bytes at a time.
    while (data + 4 <= limit) {
        uint32_t w = decodeFixed32(data);
        data += 4;
        h += w;
        h *= m;
        h ^= (h >> 16);
    }

    // pick up remaining bytes.
    switch (limit - data) {
    case 3:
        h += static_cast<unsigned char>(data[2]) << 16;
        FALLTHROUGH_INTENDED;

    case 2:
        h += static_cast<unsigned char>(data[1]) << 8;
        FALLTHROUGH_INTENDED;

    case 1:
        h += static_cast<unsigned char>(data[0]);
        h *= m;
        h ^= (h >> r);
        break;
    }

    return h;
}

uint32_t RedisDB::decodeFixed32(const char* ptr) {

    return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0]))) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16) |
        (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
}
}
